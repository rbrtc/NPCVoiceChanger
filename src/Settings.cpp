#include "Settings.h"
#include "RE/B/BGSVoiceType.h"
#include "Utils.h"
#include "pch.h"

void Settings::Load()
{
    if (!std::filesystem::exists(settingsFile))
    {
        logger::info("NPCVoiceChanger.json not found. Creating new file.");
        std::ofstream newFile{settingsFile};
        json settings{};
        settings["delay"] = delay;
        newFile << std::setw(4) << settings;
        return;
    }
    if (!IsJsonValid(settingsFile))
    {
        logger::info("Invalid json file. Skipping settings load.");
        return;
    }

    std::ifstream jsonFile{settingsFile};

    json settings{};
    jsonFile >> settings;
    jsonFile.close();

    for (const auto& [key, value] : settings.items())
    {
        if (key == "delay")
        {
            delay = value.get<unsigned>();
            continue;
        }

        const std::string npcID = key;
        const std::string newVoiceType = settings[npcID]["newVoiceType"].get<std::string>();
        const std::string oldVoiceType = settings[npcID]["oldVoiceType"].get<std::string>();
        RE::TESNPC* npc{};

        // Delimiter for form IDs
        if (npcID.contains('~'))
        {
            RE::TESForm* npcForm = Utils::GetFormFromString(npcID);
            if (!npcForm)
            {
                logger::warn("Invalid form ID: {}, skipping load of NPC.", npcID);
                continue;
            }

            npc = npcForm->As<RE::TESNPC>();
            if (!npc)
            {
                logger::warn("Invalid NPC cast: {}, skipping load of NPC.", npcID);
                continue;
            }
        }
        else
        {
            npc = RE::TESForm::LookupByEditorID<RE::TESNPC>(npcID.c_str());
            if (!npc)
            {
                logger::warn("Invalid NPC Editor ID: ", npcID);
                continue;
            }
        }

        RE::BGSVoiceType* voiceType{};

        if (!newVoiceType.empty())
        {
            voiceType = RE::TESForm::LookupByEditorID<RE::BGSVoiceType>(newVoiceType.c_str());
        }

        npc->voiceType = voiceType;
        savedNPCList[npcID] = newVoiceType;
    }
}

void Settings::AddNPC(const std::string& npcID, const std::string& newVoiceType, const std::string& oldVoiceType,
                      const std::string& name)
{
    if (npcID.empty())
    {
        logger::warn("NPC's ID is empty. Probably because the form ID attempt failed. Skipping saving to json.");
        return;
    }

    json settings{};
    json temp{};

    temp[npcID]["newVoiceType"] = newVoiceType;
    temp[npcID]["name"] = name;

    if (IsJsonValid(settingsFile))
    {
        std::ifstream jsonInput(settingsFile);
        jsonInput >> settings;
        if (settings.contains(npcID))
        {
            temp[npcID]["oldVoiceType"] = settings[npcID]["oldVoiceType"];
        }
        else
        {
            temp[npcID]["oldVoiceType"] = oldVoiceType;
        }
        settings.update(temp);
        jsonInput.close();
    }
    else
    {
        temp[npcID]["oldVoiceType"] = oldVoiceType;
        settings = temp;
    }

    std::ofstream jsonOutput(settingsFile);
    jsonOutput << std::setw(4) << settings;
    logger::info("Added NPC: {}, New voice type: {}, Original voice type: {}", npcID, newVoiceType,
                 settings[npcID]["oldVoiceType"].get<std::string>());
    savedNPCList[npcID] = newVoiceType;
}

void Settings::RemoveNPC(const std::string& npcID)
{
    if (!IsJsonValid(settingsFile))
    {
        logger::warn("Remove NPC operation cancelled: json is invalid.");
        return;
    }

    json settings{};
    std::ifstream jsonInput(settingsFile);
    jsonInput >> settings;
    jsonInput.close();

    // Reassign original voice type to NPC

    RE::TESNPC* npc{};
    RE::BGSVoiceType* voiceType{};

    // Delimiter for form IDs
    if (npcID.contains('~'))
    {
        // Checks are skipped here because Load() will only store valid NPCs

        RE::TESForm* npcForm = Utils::GetFormFromString(npcID);
        npc = npcForm->As<RE::TESNPC>();
    }
    else
    {
        npc = RE::TESForm::LookupByEditorID<RE::TESNPC>(npcID.c_str());
    }

    if (!npc)
    {
        logger::error("Removal failed for {}. Invalid NPC pointer.", npcID);
        return;
    }

    const std::string oldVoiceType = settings[npcID]["oldVoiceType"].get<std::string>();
    if (!oldVoiceType.empty())
    {
        voiceType = RE::TESForm::LookupByEditorID<RE::BGSVoiceType>(oldVoiceType.c_str());
    }
    else
    {
        voiceType = nullptr;
    }

    npc->voiceType = voiceType;

    settings.erase(npcID);
    savedNPCList.erase(npcID);
    std::ofstream jsonOutput(settingsFile);
    jsonOutput << std::setw(4) << settings;
}

bool Settings::IsJsonValid(const std::string& fileName)
{
    if (std::filesystem::exists(fileName))
    {
        std::ifstream jsonInput(fileName);
        if (json::accept(jsonInput))
        {
            jsonInput.close();
            jsonInput.open(fileName);
            if (!json::parse(jsonInput, nullptr, false).is_discarded())
            {
                return true;
            }
        }
    }
    return false;
}

std::string Settings::GetNPCName(const std::string& npcID)
{
    if (IsJsonValid(settingsFile))
    {
        std::ifstream jsonInput{settingsFile};
        json settings{};
        jsonInput >> settings;
        if (settings.contains(npcID))
        {
            return settings[npcID]["name"].get<std::string>();
        }
    }
    return "";
}

void Settings::UpdateDelay()
{
    if (!IsJsonValid(settingsFile))
    {
        logger::error("Error: invalid settings file.");
        return;
    }

    std::ifstream jsonInput{settingsFile};
    json settings{};
    jsonInput >> settings;
    jsonInput.close();
    settings["delay"] = delay;

    std::ofstream jsonOutput{settingsFile};
    jsonOutput << std::setw(4) << settings;
}
