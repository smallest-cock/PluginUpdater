#pragma once
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "version.h"
#include "Cvars.hpp"
#include <atomic>
#include <memory>

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(
    VERSION_BUILD);
constexpr auto short_plugin_version  = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);
constexpr auto pretty_plugin_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);

struct PluginData
{
	std::string name;
	std::string version;
	std::string latestReleaseUrl;
};

enum InstallStatus : uint8_t
{
	None              = 0,
	Downloading       = 1,
	Extracting        = 2,
	Copying           = 3,
	DeletingTempFiles = 5,
	AddingLineToCfg   = 6
};

enum class PluginType : uint8_t
{
	CustomQuickchat     = 0,
	CustomBallOnline    = 1,
	CustomTitle         = 2,
	CustomStatus        = 3,
	CustomGamerChair    = 4,
	CustomCar           = 5,
	CustomSalary        = 6,
	CustomWife          = 7,
	CustomSkinColor     = 8,
	StadiumDrip         = 9,
	ItemmodPresetBinder = 10,
	Unknown             = 11
};

typedef std::function<void(fs::path)> CopyFilesFunc;

class PluginUpdater : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase, public PluginWindowBase
{
	void onLoad() override;
	// void onUnload() override;

	// init
	void pluginInit();
	void initCvars();
	void initCommands();

private:
	// plugin boilerplate helper stuff
	CVarWrapper registerCvar_Bool(const CvarData& cvar, bool startingValue);
	CVarWrapper registerCvar_String(const CvarData& cvar, const std::string& startingValue);
	CVarWrapper registerCvar_Number(const CvarData& cvar, float startingValue, bool hasMinMax = false, float min = 0, float max = 0);
	CVarWrapper registerCvar_Color(const CvarData& cvar, const std::string& startingValue);
	void        registerCommand(const CvarData& cvar, const std::function<void(std::vector<std::string>)>& callback);
	CVarWrapper getCvar(const CvarData& cvar);

private:
	std::string h_label;
	fs::path    m_pluginFolder;

	std::atomic_uint8_t m_installStatus    = InstallStatus::None;
	double              m_downloadProgress = 0.0;

	// cvar values
	std::shared_ptr<int> m_loadDelay   = std::make_shared<int>(50); // in ms
	std::shared_ptr<int> m_unloadDelay = std::make_shared<int>(50); // in ms

private:
	static PluginType getPluginType(const std::string& name);
	bool              copyFiles(const std::string& name, const fs::path& zipContentsDir);
	void              addLineToCfg(const std::string& pluginNameLower);

private:
	// gui
	void display_infoTips();

private:
	// header/footer stuff
	static constexpr float header_height = 80.0f;
	static constexpr float footer_height = 40.0f;

public:
	void RenderSettings() override;
	void RenderWindow() override;
};
