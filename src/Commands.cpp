#include "pch.h"
#include "PluginUpdater.hpp"
#include "Cvars.hpp"
#include "ModUtils/util/Utils.hpp"
#include "bakkesmod/wrappers/http/HttpWrapper.h"
#include <filesystem>
#include "ScopeExit.hpp"
#include <miniz.h>

bool unzipFile(const fs::path& zipPath, const fs::path& extractDir)
{
	// Make sure output directory exists
	fs::create_directories(extractDir);

	mz_zip_archive zipArchive;
	memset(&zipArchive, 0, sizeof(zipArchive));

	// Init the zip reader
	if (!mz_zip_reader_init_file(&zipArchive, zipPath.string().c_str(), 0))
	{
		LOG("miniz: failed to open zip \"{}\"", zipPath.string());
		return false;
	}

	bool success = true;

	const int fileCount = (int)mz_zip_reader_get_num_files(&zipArchive);
	for (int i = 0; i < fileCount; ++i)
	{
		mz_zip_archive_file_stat fileStat;
		if (!mz_zip_reader_file_stat(&zipArchive, i, &fileStat))
		{
			LOG("miniz: failed to stat file {}", i);
			success = false;
			break;
		}

		fs::path outPath = extractDir / fileStat.m_filename;

		if (mz_zip_reader_is_file_a_directory(&zipArchive, i))
		{
			// Create directory
			fs::create_directories(outPath);
		}
		else
		{
			// Make sure parent directory exists
			fs::create_directories(outPath.parent_path());

			// Extract file to memory
			size_t uncompSize = 0;
			void*  p          = mz_zip_reader_extract_to_heap(&zipArchive, i, &uncompSize, 0);
			if (!p)
			{
				LOG("miniz: failed to extract \"{}\"", fileStat.m_filename);
				success = false;
				break;
			}

			// Write to disk
			std::ofstream ofs(outPath, std::ios::binary);
			ofs.write(reinterpret_cast<const char*>(p), uncompSize);
			ofs.close();

			mz_free(p);
		}
	}

	mz_zip_reader_end(&zipArchive);
	return success;
}

void PluginUpdater::initCommands()
{
	registerCommand(Commands::update,
	    [this](std::vector<std::string> args)
	    {
		    if (args.size() < 3)
		    {
			    LOG("Usage: <plugin name> <latest GH release url>");
			    return;
		    }

		    const std::string& pluginName = args[1];
		    // const std::string& pluginCurrentVersion = args[2];
		    const std::string& latestReleaseUrl = args[2];
		    const std::string  pluginNameLower  = Format::ToLower(pluginName);

		    const std::string loadCmd = std::format("plugin load {}", pluginNameLower);

		    // Step 0: unload plugin
		    const std::string unloadCmd = std::format(
		        "sleep {}; plugin unload {}; openmenu {}", *m_unloadDelay, pluginNameLower, GetMenuName());
		    cvarManager->executeCommand(unloadCmd);

		    // Step 1: Prepare temporary paths
		    fs::path tempDir = fs::temp_directory_path() / pluginName;
		    fs::create_directories(tempDir);
		    fs::path downloadFile = tempDir / "release.zip";

		    // Step 2: download latest release
		    CurlRequest req;
		    req.url               = latestReleaseUrl;
		    req.verb              = "GET";
		    req.progress_function = [this](double download_size, double downloaded, double, double)
		    { m_downloadProgress = download_size > 0.0 ? (downloaded / download_size) : 0.0; };

		    m_installStatus = InstallStatus::Downloading;
		    HttpWrapper::SendCurlRequest(req,
		        downloadFile.wstring(),
		        [this, downloadFile, pluginName, pluginNameLower](int httpCode, std::wstring filePath)
		        {
			        auto guard = make_scope_exit(
			            [this]()
			            {
				            m_installStatus = InstallStatus::None;
				            cvarManager->executeCommand("closemenu " + GetMenuName());
			            });

			        if (httpCode != 200)
			        {
				        LOGERROR("Failed to download latest release (HTTP {})", httpCode);
				        return;
			        }
			        LOG("Downloaded release zip to {}", std::string(filePath.begin(), filePath.end()));

			        try
			        {
				        // Step 3: unpack release zip
				        m_installStatus     = InstallStatus::Extracting;
				        fs::path extractDir = fs::temp_directory_path() / (pluginName + "_extracted");
				        fs::create_directories(extractDir);

				        // use miniz to extract files
				        if (!unzipFile(downloadFile, extractDir))
				        {
					        LOGERROR("Failed to unzip release package");
					        return;
				        }
				        LOG("Unpacked update to \"{}\"", extractDir.string());

				        // Step 4: install extraced files (by copying them)
				        m_installStatus = InstallStatus::Copying;
				        if (!copyFiles(pluginName, extractDir))
					        return;

				        // Step 5: clean up remaining folders in temp directory
				        m_installStatus = InstallStatus::DeletingTempFiles;
				        fs::remove_all(downloadFile.parent_path());
				        fs::remove_all(extractDir);

				        // Step 6: append line "plugin load <plugin name>" to plugins.cfg if it doesn't alr exist
				        m_installStatus = InstallStatus::AddingLineToCfg;
				        addLineToCfg(pluginNameLower);
				        LOG("Plugin {} updated successfully!", pluginName);

				        m_installStatus = InstallStatus::None;

				        // Step 7: reload plugin
				        const std::string loadCmd = std::format("sleep {}; plugin load {}; sleep 500", *m_loadDelay, pluginNameLower);
				        cvarManager->executeCommand(loadCmd);
			        }
			        catch (const std::exception& e)
			        {
				        LOGERROR("Update failed: {}", e.what());
			        }
		        });
	    });

	// testing
	registerCommand(Commands::test, [this](std::vector<std::string> args) { LOG("did test 1"); });

	registerCommand(Commands::test2, [this](std::vector<std::string> args) { LOG("did test 2"); });

	registerCommand(Commands::test3, [this](std::vector<std::string> args) { LOG("did test 3"); });
}
