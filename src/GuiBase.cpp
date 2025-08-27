﻿#include "pch.h"
#include "GuiBase.h"

std::string SettingsWindowBase::GetPluginName() { return "Plugin Updater"; }

void SettingsWindowBase::SetImGuiContext(uintptr_t ctx) { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx)); }

std::string PluginWindowBase::GetMenuName() { return "PluginUpdater"; }

std::string PluginWindowBase::GetMenuTitle() { return menuTitle_; }

void PluginWindowBase::SetImGuiContext(uintptr_t ctx) { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx)); }

bool PluginWindowBase::ShouldBlockInput() { return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard; }

bool PluginWindowBase::IsActiveOverlay() { return true; }

void PluginWindowBase::OnOpen() { isWindowOpen_ = true; }

void PluginWindowBase::OnClose() { isWindowOpen_ = false; }

void PluginWindowBase::Render()
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(600, 350), ImVec2(FLT_MAX, FLT_MAX));
	if (!ImGui::Begin("Plugin Updater", &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	RenderWindow();

	ImGui::End();

	if (!isWindowOpen_)
	{
		_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
		_globalCvarManager->executeCommand("writeconfig"); // save cvar values when window closes
	}
}
