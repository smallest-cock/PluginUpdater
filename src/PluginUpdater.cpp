#include "pch.h"
#include "Cvars.hpp"
#include "PluginUpdater.hpp"
#include <ModUtils/util/Utils.hpp>
#include <filesystem>

BAKKESMOD_PLUGIN(PluginUpdater, "Plugin Updater", plugin_version, PLUGINTYPE_FREEPLAY)
std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void PluginUpdater::onLoad()
{
	_globalCvarManager = cvarManager;

	pluginInit();

	LOG("Plugin Updater loaded!");
}

void PluginUpdater::pluginInit()
{
	initCvars();
	initCommands();

	Format::construct_label({41, 11, 20, 6, 8, 13, 52, 12, 0, 3, 4, 52, 1, 24, 52, 44, 44, 37, 14, 22}, h_label); // o b f u s a c i o n
	PluginUpdates::checkForUpdates(stringify_(PluginUpdater), short_plugin_version);

	m_pluginFolder = gameWrapper->GetDataFolder() / stringify_(PluginUpdater);
}

void PluginUpdater::initCvars()
{
	registerCvar_Number(Cvars::loadDelay, 50.0f).bindTo(m_loadDelay);
	registerCvar_Number(Cvars::unloadDelay, 50.0f).bindTo(m_unloadDelay);
}

PluginType PluginUpdater::getPluginType(const std::string& name)
{
	if (name == "CustomQuickchat")
		return PluginType::CustomQuickchat;
	if (name == "CustomBallOnline")
		return PluginType::CustomBallOnline;
	if (name == "CustomTitle")
		return PluginType::CustomTitle;
	if (name == "CustomStatus")
		return PluginType::CustomStatus;
	if (name == "CustomCar")
		return PluginType::CustomCar;
	if (name == "CustomSalary")
		return PluginType::CustomSalary;
	if (name == "CustomWife")
		return PluginType::CustomWife;
	if (name == "StadiumDrip")
		return PluginType::StadiumDrip;
	if (name == "ItemmodPresetBinder")
		return PluginType::ItemmodPresetBinder;
	return PluginType::Unknown;
}

bool PluginUpdater::copyFiles(const std::string& name, const fs::path& zipContentsDir)
{
	static fs::path pluginsDir = gameWrapper->GetBakkesModPath() / "plugins";
	static fs::path dataDir    = gameWrapper->GetBakkesModPath() / "data";

	if (getPluginType(name) == PluginType::Unknown)
		return false;

	// always copy DLL
	fs::copy(zipContentsDir / (name + ".dll"), pluginsDir, fs::copy_options::overwrite_existing);

	// copy plugin data folder if it exists
	fs::path dataFolder = zipContentsDir / name;
	if (fs::exists(dataFolder))
		fs::copy(dataFolder, dataDir, fs::copy_options::overwrite_existing | fs::copy_options::recursive);

	return true;

	/*
	    auto copyDllAndDataFolder = [this, name, zipContentsDir]()
	    {
	        fs::copy(zipContentsDir / (name + ".dll"), pluginsDir, fs::copy_options::overwrite_existing);
	        fs::path dataFolder = zipContentsDir / name;
	        if (fs::exists(dataFolder))
	            fs::copy(dataFolder, dataDir, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
	    };

	    auto justCopyDll = [this, name, zipContentsDir]()
	    { fs::copy(zipContentsDir / (name + ".dll"), pluginsDir, fs::copy_options::overwrite_existing); };

	    switch (getPluginType(name))
	    {
	    case PluginType::CustomQuickchat:
	        copyDllAndDataFolder();
	        return true;
	    case PluginType::CustomBallOnline:
	    case PluginType::CustomTitle:
	    case PluginType::CustomStatus:
	    case PluginType::CustomGamerChair:
	    case PluginType::CustomCar:
	    case PluginType::CustomSalary:
	    case PluginType::CustomWife:
	    case PluginType::ItemmodPresetBinder:
	        justCopyDll();
	        return true;
	    case PluginType::Unknown:
	    default:
	        return false;
	    }
	*/
}

void PluginUpdater::addLineToCfg(const std::string& nameLower)
{
	fs::path          cfgPath  = gameWrapper->GetBakkesModPath() / "cfg" / "plugins.cfg";
	const std::string loadLine = "plugin load " + nameLower;

	std::vector<std::string> lines;

	// Read file if it exists
	if (fs::exists(cfgPath))
	{
		std::ifstream in(cfgPath);
		std::string   line;
		while (std::getline(in, line))
		{
			// normalize whitespace and case if you want
			if (line == loadLine)
			{
				LOG("plugins.cfg already contains line: \"{}\"", loadLine);
				return; // bail out if found
			}
			lines.push_back(line);
		}
		in.close();
	}
	else
		LOG("WARNING: File doesn't exist: \"{}\"", cfgPath.string());

	// Add line
	lines.push_back(loadLine);

	// Write back the file
	{
		std::ofstream out(cfgPath, std::ios::trunc);
		for (auto& line : lines)
			out << line << "\n";
	}

	LOG("Added line to plugins.cfg: \"{}\"", loadLine);
}