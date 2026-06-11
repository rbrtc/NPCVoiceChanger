#include "Strings.h"
#include "pch.h"
#include <SimpleIni.h>

std::unordered_map<std::string, std::string> Strings::_map;

void Strings::Load()
{
    _map.clear();

    _map["ModName"] = "NPC Voice Changer";
    _map["SettingsLabel"] = "Settings";
    _map["NPCSection"] = "NPC Selection";
    _map["VoiceTypeSection"] = "Voice Type Selection";
    _map["VoiceTypeFilter"] = "Voice Type Filter";
    _map["VoiceTypeFilterHelp"] = "Type part of a voice type editorID or filename to filter the dropdown list below.";
    _map["VoiceTypeDropdown"] = "Voice Type";
    _map["VoiceTypeDropdownHelp"] = "Select a voice type from the dropdown list.";
    _map["NoVoiceTypeMatches"] = "<no cached voice type matches>";
    _map["SaveLabel"] = "Save";
    _map["RefButton"] = "Get Selected NPC from the Console";
    _map["NoRefError"] = "No NPC selected";
    _map["NotNPCError"] = "Selected ref is not an NPC!";
    _map["NPCSelectedText"] = "selected.";
    _map["RefHelp"] = "Get the selected reference from the console. Make sure you are selecting an NPC.";
    _map["VoiceTypeError"] = "No voice type selected";
    _map["ModifiedSection"] = "NPCs Modified";
    _map["DeleteButton"] = "X";

    constexpr auto path = "Data/SKSE/Plugins/NPCVoiceChanger_Translation.ini";
    if (!std::filesystem::exists(path))
    {
        logger::info("Strings: translation file missing, using built-in English");
        return;
    }

    CSimpleIniA ini;
    ini.SetUnicode();
    if (ini.LoadFile(path) != SI_OK)
    {
        logger::warn("Strings: translation file exists but failed to parse");
        return;
    }

    CSimpleIniA::TNamesDepend keys;
    if (ini.GetAllKeys("Strings", keys))
    {
        for (const auto &key : keys)
        {
            if (const char *val = ini.GetValue("Strings", key.pItem))
            {
                _map[key.pItem] = val;
            }
        }
    }

    logger::info("Strings: translation overrides loaded");
}

const char *Strings::Get(std::string_view a_key)
{
    const auto it = _map.find(std::string(a_key));
    return it != _map.end() ? it->second.c_str() : "";
}
