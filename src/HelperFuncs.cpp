#include "pch.h"
#include "PluginUpdater.hpp"
#include "Cvars.hpp"

// cvars
CVarWrapper PluginUpdater::registerCvar_Bool(const CvarData& cvar, bool startingValue)
{
	std::string value = startingValue ? "1" : "0";

	return cvarManager->registerCvar(cvar.name, value, cvar.description, true, true, 0, true, 1);
}

CVarWrapper PluginUpdater::registerCvar_String(const CvarData& cvar, const std::string& startingValue)
{
	return cvarManager->registerCvar(cvar.name, startingValue, cvar.description);
}

CVarWrapper PluginUpdater::registerCvar_Number(const CvarData& cvar, float startingValue, bool hasMinMax, float min, float max)
{
	std::string numberStr = std::to_string(startingValue);

	if (hasMinMax)
	{
		return cvarManager->registerCvar(cvar.name, numberStr, cvar.description, true, true, min, true, max);
	}
	else
	{
		return cvarManager->registerCvar(cvar.name, numberStr, cvar.description);
	}
}

CVarWrapper PluginUpdater::registerCvar_Color(const CvarData& cvar, const std::string& startingValue)
{
	return cvarManager->registerCvar(cvar.name, startingValue, cvar.description);
}

void PluginUpdater::registerCommand(const CvarData& cvar, const std::function<void(std::vector<std::string>)>& callback)
{
	cvarManager->registerNotifier(cvar.name, callback, cvar.description, PERMISSION_ALL);
}

CVarWrapper PluginUpdater::getCvar(const CvarData& cvar) { return cvarManager->getCvar(cvar.name); }
