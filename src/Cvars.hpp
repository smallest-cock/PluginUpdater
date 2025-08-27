#pragma once

#define CVAR(name, desc) CvarData("pluginupdater_" name, desc) // automatically apply the prefix to cvar names

struct CvarData
{
	const char* name;
	const char* description;

	constexpr CvarData(const char* name, const char* description) : name(name), description(description) {}
};

namespace Cvars
{
// bools
constexpr CvarData enabled = CVAR("enabled", "use plugin updater");

// numbers
constexpr CvarData unloadDelay = CVAR("unload_delay", "unload plugin delay (in milliseconds)");
constexpr CvarData loadDelay   = CVAR("load_delay", "load plugin delay (in milliseconds)");

// strings

// colors
} // namespace Cvars

namespace Commands
{
constexpr CvarData update = CVAR(
    "update", "update a plugin. args: <plugin name lowercase> <plugin current version> <latest GH release url>");
constexpr CvarData test  = CVAR("test", "test");
constexpr CvarData test2 = CVAR("test2", "test 2");
constexpr CvarData test3 = CVAR("test3", "test 3");
} // namespace Commands