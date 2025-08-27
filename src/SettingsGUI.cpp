#include "pch.h"
#include "PluginUpdater.hpp"
#include "Cvars.hpp"
#include "ModUtils/gui/GuiTools.hpp"

void PluginUpdater::RenderSettings()
{
	const float content_height = ImGui::GetContentRegionAvail().y - footer_height; // available height after accounting for footer

	if (ImGui::BeginChild("MainSettingsSection", ImVec2(0, content_height)))
	{
		GUI::alt_settings_header(h_label.c_str(), pretty_plugin_version, gameWrapper, true); // what a regular plugin would use

		GUI::Spacing(4);

		auto unloadDelay_cvar = getCvar(Cvars::unloadDelay);
		auto loadDelay_cvar   = getCvar(Cvars::loadDelay);
		if (unloadDelay_cvar)
		{
			int unloadDelay = unloadDelay_cvar.getIntValue();
			if (ImGui::SliderInt("Plugin unload delay", &unloadDelay, 0, 500, "%d ms"))
				unloadDelay_cvar.setValue(unloadDelay);
			GUI::ToolTip("Helps prevent crashes");

			int loadDelay = loadDelay_cvar.getIntValue();
			if (ImGui::SliderInt("Plugin load delay", &loadDelay, 0, 500, "%d ms"))
				loadDelay_cvar.setValue(loadDelay);
			GUI::ToolTip("Helps prevent crashes");
		}

		GUI::Spacing(8);

		display_infoTips();
	}
	ImGui::EndChild();

	GUI::alt_settings_footer("Need help? Join the Discord", "https://discord.gg/d5ahhQmJbJ");
}

void PluginUpdater::RenderWindow()
{
	if (m_installStatus == InstallStatus::None)
		return;

	GUI::Spacing(6);

	static const ImVec2 progressBarSize = ImVec2(200, 0);

	switch (m_installStatus)
	{
	case InstallStatus::Downloading:
		ImGui::Text("Downloading latest release...");
		GUI::Spacing(2);
		ImGui::ProgressBar(static_cast<float>(m_downloadProgress * 0.7));
		break;
	case InstallStatus::Extracting:
		ImGui::Text("Extracting zip...");
		GUI::Spacing(2);
		ImGui::ProgressBar(0.7f);
		break;
	case InstallStatus::Copying:
		ImGui::Text("Copying files...");
		GUI::Spacing(2);
		ImGui::ProgressBar(0.8f);
		break;
	case InstallStatus::DeletingTempFiles:
		ImGui::Text("Cleaning temp files...");
		GUI::Spacing(2);
		ImGui::ProgressBar(0.9f);
		break;
	case InstallStatus::AddingLineToCfg:
		ImGui::Text("Adding line to plugins.cfg...");
		GUI::Spacing(2);
		ImGui::ProgressBar(1.0f);
		break;
	default:
		break;
	}
}

void PluginUpdater::display_infoTips()
{
	ImGui::TextColored(GUI::Colors::LightGreen, "This is a convenience plugin");
	ImGui::BulletText("If it doesn't work, just manually update by installing the latest version from GitHub");
}