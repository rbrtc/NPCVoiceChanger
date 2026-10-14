#include "UI.h"
#include "SKSEMenuFramework.h"
#include "Strings.h"
#include "Settings.h"
#include "Utils.h"
#include "pch.h"
#include <CLIBUtil/editorID.hpp>
#include <CLIBUtil/string.hpp>

// Pushes the help marker to the right edge of the window
// and displays a tooltip with the text parameter
void UI::PrintHelpMarker(const char* text)
{
    const float textWidth = ImGuiMCP::CalcTextSize(helpMarker.c_str()).x;
    const float availWidth = ImGuiMCP::GetContentRegionAvail().x;
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + (availWidth - textWidth));
    ImGuiMCP::TextDisabled("%s", helpMarker.c_str());

    if (ImGuiMCP::BeginItemTooltip())
    {
        ImGuiMCP::PushTextWrapPos(ImGuiMCP::GetFontSize() * 35.0f);
        ImGuiMCP::TextUnformatted(text);
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
    BuildCache();
}

void __stdcall UI::RenderMenu()
{
    // NPC Selection

    ImGuiMCP::SeparatorText(Strings::Get("NPCSection"));

    static std::string refDisplayText{};
    static std::string vtDisplayText{};
    static std::string vtEditorID{};
    static const std::string dynamicFormWarning = Strings::Get("DynamicFormWarning");
    static RE::TESNPC* selectedNPC{};

    if (ImGuiMCP::Button(Strings::Get("RefButton")))
    {
        selectedNPC = nullptr;
        refDisplayText = "";
        vtDisplayText = "";
        vtEditorID = "";

        if (const auto& ref = RE::Console::GetSelectedRef())
        {
            if (auto* actorRef = ref->As<RE::Actor>())
            {
                selectedNPC = actorRef->GetActorBase();
                if (selectedNPC)
                {
                    refDisplayText = std::format("{} {}", selectedNPC->GetName(), Strings::Get("NPCSelectedText"));

                    if (selectedNPC->voiceType)
                    {
                        vtEditorID = clib_util::editorID::get_editorID(selectedNPC->voiceType);
                        vtDisplayText = std::format("{}: {}", Strings::Get("CurrentVT"), vtEditorID);
                    }
                    else
                    {
                        vtEditorID = "";
                        vtDisplayText = std::format("{}: {}", Strings::Get("CurrentVT"), Strings::Get("Unknown"));
                    }
                }
                else
                {
                    logger::error("{} could not be casted to TESNPC", actorRef->GetName());
                }
            }
            else
            {
                refDisplayText = Strings::Get("NotNPCError");
            }
        }
        else
        {
            refDisplayText = Strings::Get("NoRefError");
        }
    }
    ImGuiMCP::SameLine();
    PrintHelpMarker(Strings::Get("RefHelp"));

    if (!refDisplayText.empty())
    {
        ImGuiMCP::TextUnformatted(refDisplayText.c_str());
    }
    if (!vtDisplayText.empty())
    {
        ImGuiMCP::TextUnformatted(vtDisplayText.c_str());
    }
    if (selectedNPC && selectedNPC->IsDynamicForm())
    {
        ImGuiMCP::TextUnformatted(dynamicFormWarning.c_str());
    }
    ImGuiMCP::NewLine();

    // Voice Type Section

    ImGuiMCP::SeparatorText(Strings::Get("VoiceTypeSection"));

    static std::string selectedEntry{};

    if (ImGuiMCP::BeginCombo("##vtList", selectedEntry.c_str()))
    {
        static ImGuiMCP::ImGuiTextFilter vtFilter{};
        if (ImGuiMCP::IsWindowAppearing())
        {
            ImGuiMCP::SetKeyboardFocusHere();
            vtFilter.Clear();
        }
        ImGuiMCP::SetNextItemShortcut(ImGuiMCP::ImGuiMod_Ctrl | ImGuiMCP::ImGuiKey_F);
        vtFilter.Draw("##vtFilter", -FLT_MIN);

        for (const auto& voiceType : voiceTypeList)
        {
            const bool isSelected = selectedEntry == voiceType;

            if (vtFilter.PassFilter(voiceType.c_str()))
            {
                if (ImGuiMCP::Selectable(voiceType.c_str(), isSelected))
                {
                    selectedEntry = voiceType;
                }
            }
        }
        ImGuiMCP::EndCombo();
    }

    // Save button

    static std::string saveDisplayText{};
    static RE::BGSVoiceType* selectedVoiceType{};

    ImGuiMCP::SameLine();

    if (ImGuiMCP::Button(Strings::Get("SaveLabel")))
    {
        if (!selectedEntry.empty())
        {
            const auto splitStrings = clib_util::string::split(selectedEntry, " | ");
            selectedVoiceType = RE::TESForm::LookupByEditorID<RE::BGSVoiceType>(splitStrings[0]);
        }
        if (selectedNPC && selectedVoiceType)
        {
            // Get orignal voice type before switching to new one,
            // then store both in the json file as well as the NPC's editor ID

            std::string npcID = clib_util::editorID::get_editorID(selectedNPC);
            if (npcID.empty())
            {
                npcID = Utils::NormalizeFormID(selectedNPC->GetAsForm());
            }

            const std::string name = selectedNPC->GetName();
            const std::string newVoiceType = clib_util::editorID::get_editorID(selectedVoiceType);
            const std::string oldVoiceType =
                selectedNPC->voiceType ? clib_util::editorID::get_editorID(selectedNPC->voiceType) : "";

            logger::info("Attempting to add NPC: {} New voice type: {} Old voice type: {}", npcID, newVoiceType,
                         oldVoiceType);

            if (!selectedNPC->IsDynamicForm())
            {
                Settings::AddNPC(npcID, newVoiceType, oldVoiceType, name);
            }
            else
            {
                logger::info("Skipped adding {} to the settings file because it is a dynamic form.", npcID);
            }

            selectedNPC->voiceType = selectedVoiceType;
            vtEditorID = newVoiceType;
            vtDisplayText = std::format("{}: {}", Strings::Get("CurrentVT"), vtEditorID);
        }
        else if (!selectedNPC && !selectedVoiceType)
        {
            saveDisplayText = std::format("{}\n{}", Strings::Get("VoiceTypeError"), Strings::Get("NoRefError"));
        }
        else if (!selectedVoiceType)
        {
            saveDisplayText = Strings::Get("VoiceTypeError");
        }
        else if (!selectedNPC)
        {
            saveDisplayText = Strings::Get("NoRefError");
        }
    }

    ImGuiMCP::SameLine();
    PrintHelpMarker(Strings::Get("VoiceTypeDropdownHelp"));

    if (!saveDisplayText.empty())
    {
        ImGuiMCP::TextUnformatted(saveDisplayText.c_str());
    }
    ImGuiMCP::NewLine();

    // Input for delay

    ImGuiMCP::SeparatorText(Strings::Get("Delay"));

    static const ImGuiMCP::ImU8 step{1};
    ImGuiMCP::SetNextItemWidth(150.0f);
    if (ImGuiMCP::InputScalar("##delayInput", ImGuiMCP::ImGuiDataType_U8, &Settings::delay, &step))
    {
        Settings::UpdateDelay();
    }
    ImGuiMCP::SameLine();
    ImGuiMCP::TextUnformatted(Strings::Get("Seconds"));
    ImGuiMCP::SameLine();
    PrintHelpMarker(Strings::Get("DelayHelp"));
    ImGuiMCP::NewLine();

    // Modified NPCs Section

    ImGuiMCP::SeparatorText(Strings::Get("ModifiedSection"));

    // key: npcID, value: newVoiceType
    const std::map<std::string, std::string> savedNPCList = Settings::GetNPCList();
    std::string toRemove{};
    for (const auto& savedNPC : savedNPCList)
    {
        const std::string name = Settings::GetNPCName(savedNPC.first);
        std::string savedNPCInfo{};
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

        if (ImGuiMCP::Button("X"))
        {
            toRemove = savedNPC.first;
        }
        ImGuiMCP::PopID();
    }
    if (!toRemove.empty())
    {
        Settings::RemoveNPC(toRemove);

        if (!selectedNPC)
        {
            return;
        }

        // Updates the currently selected NPC's displayed voice type

        const std::string npcID = clib_util::editorID::get_editorID(selectedNPC);
        if (npcID != toRemove)
        {
            return;
        }

        const std::string currentVT =
            selectedNPC->voiceType ? clib_util::editorID::get_editorID(selectedNPC->voiceType) : "";
        if (currentVT != vtEditorID)
        {
            vtEditorID = currentVT;
            vtDisplayText = std::format("{}: {}", Strings::Get("CurrentVT"),
                                        vtEditorID.empty() ? Strings::Get("Unknown") : vtEditorID);
        }
    }
}

void UI::BuildCache()
{
    const auto allVoiceTypes = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::BGSVoiceType>();
    for (const auto& voiceType : allVoiceTypes)
    {
        const std::string editorID = clib_util::editorID::get_editorID(voiceType);
        if (!editorID.empty())
        {
            const std::string fileName = static_cast<std::string>(voiceType->GetFile()->GetFilename());
            voiceTypeList.push_back(editorID + " | " + fileName);
        }
    }

    std::sort(voiceTypeList.begin(), voiceTypeList.end());
}
