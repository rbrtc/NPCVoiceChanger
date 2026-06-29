#include "UI.h"
#include "SKSEMenuFramework.h"
#include "Strings.h"
#include "Utils.h"
#include "VoiceTypeRegistry.h"
#include "pch.h"
#include <CLIBUtil/editorID.hpp>

namespace
{
char voiceTypeFilterBuffer[256]{};
char voiceTypeDropdownBuffer[256]{};
char refBuffer[256]{};
char voiceTypeBuffer[256]{};

RE::TESNPC *selectedNPC{nullptr};
RE::BGSVoiceType *selectedVoiceType{nullptr};

} // namespace

void UI::HelpMarker(const char *a_desc)
{
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{0.55f, 0.55f, 0.55f, 1.0f});
    ImGuiMCP::TextUnformatted("(?)");
    ImGuiMCP::PopStyleColor();
    if (ImGuiMCP::IsItemHovered())
    {
        ImGuiMCP::BeginTooltip();
        ImGuiMCP::PushTextWrapPos(ImGuiMCP::GetFontSize() * 28.0f);
        ImGuiMCP::TextUnformatted(a_desc);
        ImGuiMCP::PopTextWrapPos();
        ImGuiMCP::EndTooltip();
    }
}

void UI::Register()
{
    if (!SKSEMenuFramework::IsInstalled())
    {
        logger::warn("UI: SKSE Menu Framework not installed");
        return;
    }

    SKSEMenuFramework::SetSection(Strings::Get("ModName"));
    SKSEMenuFramework::AddSectionItem(Strings::Get("SettingsLabel"), RenderMenu);
    logger::info("UI: registered with SKSE Menu Framework");
}

void __stdcall UI::RenderMenu()
{
    auto &settings = Settings::GetSingleton();
    auto &voiceTypeRegistry = VoiceTypeRegistry::GetSingleton();

    // NPC Selection

    ImGuiMCP::SeparatorText(Strings::Get("NPCSection"));

    if (ImGuiMCP::Button(Strings::Get("RefButton")))
    {
        const auto &ref = RE::Console::GetSelectedRef();

        if (!ref)
        {
            std::strncpy(refBuffer, Strings::Get("NoRefError"), sizeof(refBuffer) - 1);
            refBuffer[sizeof(refBuffer) - 1] = '\0';
        }
        else
        {
            RE::Actor *actorRef = ref->As<RE::Actor>();

            if (actorRef)
            {
                selectedNPC = actorRef->GetActorBase();
                std::string npcInfo = selectedNPC->GetName();

                npcInfo += std::format(" {}", Strings::Get("NPCSelectedText"));

                std::strncpy(refBuffer, npcInfo.c_str(), sizeof(refBuffer) - 1);
                refBuffer[sizeof(refBuffer) - 1] = '\0';
            }
            else
            {
                std::strncpy(refBuffer, Strings::Get("NotNPCError"), sizeof(refBuffer) - 1);
                refBuffer[sizeof(refBuffer) - 1] = '\0';
            }
        }
    }
    ImGuiMCP::SameLine();
    HelpMarker(Strings::Get("RefHelp"));
    ImGuiMCP::TextUnformatted(refBuffer);

    ImGuiMCP::SeparatorText(Strings::Get("VoiceTypeSection"));

    ImGuiMCP::TextUnformatted(Strings::Get("VoiceTypeFilter"));
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(ImGuiMCP::GetWindowWidth() * 0.35f);
    ImGuiMCP::InputText("##voiceType_filter", voiceTypeFilterBuffer, sizeof(voiceTypeFilterBuffer));
    ImGuiMCP::SameLine();
    HelpMarker(Strings::Get("VoiceTypeFilterHelp"));

    // Voice Type Section

    const auto voiceTypeMatches = voiceTypeRegistry.Filter(voiceTypeFilterBuffer);
    const auto &voiceTypeEntries = voiceTypeRegistry.GetEntries();
    const char *voiceTypePreview =
        voiceTypeDropdownBuffer[0] != '\0' ? voiceTypeDropdownBuffer : Strings::Get("VoiceTypeError");

    ImGuiMCP::TextUnformatted(Strings::Get("VoiceTypeDropdown"));
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(ImGuiMCP::GetWindowWidth() * 0.45f);
    if (ImGuiMCP::BeginCombo("##voiceType_dropdown", voiceTypePreview, ImGuiMCP::ImGuiComboFlags_HeightLarge))
    {
        if (voiceTypeMatches.empty())
        {
            (void)ImGuiMCP::Selectable(Strings::Get("NoVoiceTypeMatches"), false,
                                       ImGuiMCP::ImGuiSelectableFlags_Disabled);
        }
        else
        {
            for (const std::size_t index : voiceTypeMatches)
            {
                const auto &entry = voiceTypeEntries[index];
                // displayName is file name in this case
                std::string voiceTypeDisplayed = entry.editorID + " | " + entry.displayName;

                if (ImGuiMCP::Selectable(voiceTypeDisplayed.c_str(), false))
                {
                    std::strncpy(voiceTypeDropdownBuffer, entry.editorID.c_str(), sizeof(voiceTypeDropdownBuffer) - 1);
                    voiceTypeDropdownBuffer[sizeof(voiceTypeDropdownBuffer) - 1] = '\0';
                    selectedVoiceType = RE::TESForm::LookupByID<RE::BGSVoiceType>(entry.formID);
                    std::strncpy(voiceTypeBuffer, "", sizeof(voiceTypeBuffer) - 1);
                    voiceTypeBuffer[sizeof(voiceTypeBuffer) - 1] = '\0';
                }
            }
        }
        ImGuiMCP::EndCombo();
    }

    ImGuiMCP::SameLine();
    HelpMarker(Strings::Get("VoiceTypeDropdownHelp"));

    // Save button

    ImGuiMCP::SameLine();

    if (ImGuiMCP::Button(Strings::Get("SaveLabel")))
    {
        if (selectedNPC && selectedVoiceType)
        {
            // Get orignal voice type before switching to new one,
            // then store both in the json file as well as the NPC's editor ID

            std::string npcID = clib_util::editorID::get_editorID(selectedNPC);
            if (npcID.empty())
            {
                logger::warn("Selected NPC does not have an editor ID! Attempting to use form ID instead.");
                npcID = Utils::NormalizeFormID(selectedNPC->GetAsForm());
            }

            const std::string name = selectedNPC->GetName();
            const std::string newVoiceType = clib_util::editorID::get_editorID(selectedVoiceType);
            std::string oldVoiceType;

            if (selectedNPC->voiceType)
            {
                oldVoiceType = clib_util::editorID::get_editorID(selectedNPC->voiceType);
            }
            else
            {
                oldVoiceType = newVoiceType;
            }

            logger::info("Attempting to add NPC: {} New voice type: {} Old voice type: {}", npcID, newVoiceType,
                         oldVoiceType);
            Settings::GetSingleton().AddNPC(npcID, newVoiceType, oldVoiceType, name);

            selectedNPC->voiceType = selectedVoiceType;
        }
        else if (!selectedVoiceType)
        {
            std::strncpy(voiceTypeBuffer, Strings::Get("VoiceTypeError"), sizeof(voiceTypeBuffer) - 1);
            voiceTypeBuffer[sizeof(voiceTypeBuffer) - 1] = '\0';
        }
    }

    // Voice type error message
    ImGuiMCP::TextUnformatted(voiceTypeBuffer);

    // Modified NPCs Section

    ImGuiMCP::SeparatorText(Strings::Get("ModifiedSection"));

    const std::map<std::string, std::string> savedNPCList = Settings::GetSingleton().GetNPCList();
    std::string toRemove{};
    for (const auto &savedNPC : savedNPCList)
    {
        const std::string name = Settings::GetSingleton().GetNPCName(savedNPC.first);
        std::string savedNPCInfo;
        if (!name.empty())
        {
            savedNPCInfo = name + " | " + savedNPC.first + " | " + savedNPC.second;
        }
        else
        {
            savedNPCInfo = savedNPC.first + " | " + savedNPC.second;
        }

        ImGuiMCP::TextUnformatted(savedNPCInfo.c_str());
        ImGuiMCP::SameLine();
        ImGuiMCP::PushID(savedNPC.first.c_str());
        if (ImGuiMCP::Button(Strings::Get("DeleteButton")))
        {
            toRemove = savedNPC.first;
        }
        ImGuiMCP::PopID();
    }
    if (!toRemove.empty())
    {
        Settings::GetSingleton().RemoveNPC(toRemove);
    }
}
