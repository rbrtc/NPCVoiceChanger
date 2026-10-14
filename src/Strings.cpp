#include "Strings.h"
#include "pch.h"
#include "Settings.h"
#include <nlohmann/json.hpp>
#include <unordered_map>

void Strings::Load()
{
    stringMap["ModName"] = "NPC Voice Changer";
    stringMap["SettingsLabel"] = "Settings";
    stringMap["Delay"] = "Delay";
    stringMap["DelayHelp"] = "Loads the settings X seconds after loading a game.";
    stringMap["Seconds"] = "Seconds";
    stringMap["NPCSection"] = "NPC Selection";
    stringMap["VoiceTypeSection"] = "Voice Type Selection";
    stringMap["VoiceTypeDropdownHelp"] = "Select a voice type from the dropdown list.";
    stringMap["SaveLabel"] = "Save";
    stringMap["RefButton"] = "Get Selected NPC from the Console";
    stringMap["RefHelp"] = "Get the selected reference from the console. Make sure you are selecting an NPC.";
    stringMap["NoRefError"] = "No NPC selected";
    stringMap["NotNPCError"] = "Selected ref is not an NPC!";
    stringMap["NPCSelectedText"] = "selected.";
    stringMap["VoiceTypeError"] = "No voice type selected";
    stringMap["ModifiedSection"] = "Modified NPCs";
    stringMap["CurrentVT"] = "Current voice type";
    stringMap["Unknown"] = "Unknown";
    stringMap["DynamicFormWarning"] =
        "Warning: the selected NPC is a dynamic form. Changes to its voice type will not be saved.";

    constexpr auto path = "Data/SKSE/Plugins/NPCVoiceChanger_Translation.json";
    if (!Settings::IsJsonValid(path))
    {
        logger::warn("Translation file missing or JSON parse error detected. Using built-in English.");
        return;
    }

    json translations{};
    std::ifstream jsonFile{path};
    jsonFile >> translations;

    for (const auto& [key, value] : translations.items())
    {
        stringMap[key] = value;
    }

    logger::info("Strings: translation overrides loaded");
}

const char* Strings::Get(const std::string& key)
{
    const auto it = stringMap.find(key);
    return it != stringMap.end() ? it->second.c_str() : "";
}
