#include "Settings.h"
#include "RE/B/BGSVoiceType.h"
#include "Utils.h"
#include "pch.h"

namespace
{

void EnsurePluginDirectory()
{
    std::error_code ec;
    std::filesystem::create_directories(L"Data/SKSE/Plugins", ec);
    if (ec)
    {
        logger::warn("Settings: failed to create Data/SKSE/Plugins: {}", ec.message());
    }
}

} // namespace

Settings &Settings::GetSingleton()
{
    static Settings settings;
    return settings;
}

void Settings::Load()
{
    // TODO: Keep track of loaded NPCs in the UI with an option to remove them

    EnsurePluginDirectory();

    if (!std::filesystem::exists(jsonFileName))
    {
        logger::info("NPCVoiceChanger.json not found. Skipping settings load.");
        return;
    }
    if (!IsJsonValid())
    {
        logger::info("Invalid json file. Skipping settings load.");
        return;
    }

    std::ifstream jsonFile(jsonFileName);

    json settings;
    jsonFile >> settings;
    jsonFile.close();

    for (auto &[key, value] : settings.items())
    {
        std::string npcID = key;
        const std::string newVoiceType = settings[npcID]["newVoiceType"].get<std::string>();
        const std::string oldVoiceType = settings[npcID]["oldVoiceType"].get<std::string>();

        if (npcID.empty())
        {
            logger::warn("NPC ID is empty, skipping load of NPC.");
            continue;
        }

        if (newVoiceType.empty() || oldVoiceType.empty())
        {
            logger::warn("{} for {} is empty, skipping load of NPC.",
                         newVoiceType.empty() ? "newVoiceType" : "oldVoiceType", npcID);
            continue;
        }

        RE::TESNPC *npc;

        // Delimiter for form IDs
        if (npcID.contains('~'))
        {
            RE::TESForm *npcForm = Utils::GetFormFromString(npcID);
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
                logger::warn("Invalid NPC pointer when trying to look up editor ID, skipping load of NPC: {}.", npcID);
                continue;
            }
        }

        RE::BGSVoiceType *voiceType = RE::TESForm::LookupByEditorID<RE::BGSVoiceType>(newVoiceType.c_str());

        if (!voiceType)
        {
            logger::warn("Invalid voice type pointer for NPC: {} with voice type: {}. Skipping load of NPC.", npcID,
                         newVoiceType);
            continue;
        }

        npc->voiceType = voiceType;
        savedNPCList[npcID] = newVoiceType;
    }
}

void Settings::AddNPC(const std::string &npcID, const std::string &newVoiceType, const std::string &oldVoiceType,
                      const std::string &name)
{
    if (npcID.empty())
    {
        logger::warn("NPC's ID is empty. Probably because the form ID attempt failed. Skipping saving to json.");
        return;
    }

    json settings, temp;

    temp[npcID]["newVoiceType"] = newVoiceType;
    temp[npcID]["name"] = name;

    if (IsJsonValid())
    {
        std::ifstream jsonInput(jsonFileName);
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

    std::ofstream jsonOutput(jsonFileName);
    jsonOutput << std::setw(4) << settings;
    jsonOutput.close();
    logger::info("Added NPC: {}, New voice type: {}, Original voice type: {}", npcID, newVoiceType,
                 settings[npcID]["oldVoiceType"].get<std::string>());
    savedNPCList[npcID] = newVoiceType;
}

void Settings::RemoveNPC(const std::string &npcID)
{
    if (!IsJsonValid())
    {
        logger::warn("Remove NPC operation cancelled: json is invalid.");
        return;
    }

    json settings;
    std::ifstream jsonInput(jsonFileName);
    jsonInput >> settings;
    jsonInput.close();

    // Reassign original voice type to NPC

    RE::TESNPC *npc;
    RE::BGSVoiceType *voiceType;

    // Delimiter for form IDs
    if (npcID.contains('~'))
    {
        // Checks are skipped here because Load() will only store valid NPCs

        RE::TESForm *npcForm = Utils::GetFormFromString(npcID);
        npc = npcForm->As<RE::TESNPC>();
    }
    else
    {
        npc = RE::TESForm::LookupByEditorID<RE::TESNPC>(npcID.c_str());
    }

    voiceType =
        RE::TESForm::LookupByEditorID<RE::BGSVoiceType>(settings[npcID]["oldVoiceType"].get<std::string>().c_str());

    if (npc && voiceType)
    {
        npc->voiceType = voiceType;
    }
    else
    {
        logger::warn("Removal failed for {}. Invalid NPC and/or voice type pointer.", npcID);
        return;
    }

    settings.erase(npcID);
    savedNPCList.erase(npcID);
    std::ofstream jsonOutput(jsonFileName);
    jsonOutput << std::setw(4) << settings;
    jsonOutput.close();
}

const bool Settings::IsJsonValid() const
{
    if (std::filesystem::exists(jsonFileName))
    {
        std::ifstream jsonInput(jsonFileName);
        if (json::accept(jsonInput))
        {
            jsonInput.close();
            jsonInput.open(jsonFileName);
            if (!json::parse(jsonInput, nullptr, false).is_discarded())
            {
                jsonInput.close();
                return true;
            }
        }
        jsonInput.close();
    }
    return false;
}

const std::string Settings::GetNPCName(const std::string &npcID) const
{
    json settings;
    std::ifstream jsonInput(jsonFileName);

    if (IsJsonValid())
    {
        jsonInput >> settings;

        if (settings.contains(npcID))
        {
            jsonInput.close();
            return settings[npcID]["name"].get<std::string>();
        }
    }

    jsonInput.close();
    return "";
}
