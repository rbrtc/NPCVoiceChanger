#include "VoiceTypeRegistry.h"
#include "Settings.h"
#include "pch.h"

VoiceTypeRegistry &VoiceTypeRegistry::GetSingleton()
{
    static VoiceTypeRegistry registry;
    return registry;
}

std::string VoiceTypeRegistry::ToLower(std::string_view a_value)
{
    std::string result(a_value);
    std::ranges::transform(result, result.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

void VoiceTypeRegistry::BuildCache()
{
    if (_built)
    {
        return;
    }

    _entries.clear();

    const auto &[editorIDMap, mapLock] = RE::TESForm::GetAllFormsByEditorID();
    if (editorIDMap)
    {
        _entries.reserve(editorIDMap->size());
        for (const auto &[editorID, form] : *editorIDMap)
        {
            if (!form)
            {
                continue;
            }

            auto *voiceType = form->As<RE::BGSVoiceType>();
            if (!voiceType || editorID.empty())
            {
                continue;
            }

            std::string name = static_cast<std::string>(voiceType->GetFile()->GetFilename());
            VoiceTypeEntry entry;
            entry.editorID = editorID.c_str();
            entry.editorIDLower = ToLower(entry.editorID);
            entry.displayName = (name[0] != '\0') ? name : entry.editorID;
            entry.displayNameLower = ToLower(entry.displayName);
            entry.formID = voiceType->GetFormID();
            _entries.push_back(std::move(entry));
        }
    }

    std::ranges::sort(_entries, [](const VoiceTypeEntry &a_lhs, const VoiceTypeEntry &a_rhs) {
        if (a_lhs.displayNameLower != a_rhs.displayNameLower)
        {
            return a_lhs.displayNameLower < a_rhs.displayNameLower;
        }
        return a_lhs.editorIDLower < a_rhs.editorIDLower;
    });

    _built = true;
    if (_entries.empty())
    {
        logger::warn("VoiceTypeRegistry: no voice type entries found (editorIDMap was null or empty)");
    }
}

const std::vector<VoiceTypeEntry> &VoiceTypeRegistry::GetEntries() const
{
    return _entries;
}

std::vector<std::size_t> VoiceTypeRegistry::Filter(std::string_view a_query) const
{
    std::vector<std::size_t> matches;
    const std::string needle = ToLower(a_query);
    matches.reserve(_entries.size());

    for (std::size_t i = 0; i < _entries.size(); ++i)
    {
        const auto &entry = _entries[i];
        if (needle.empty() || entry.editorIDLower.find(needle) != std::string::npos ||
            entry.displayNameLower.find(needle) != std::string::npos)
        {
            matches.push_back(i);
        }
    }

    return matches;
}
