#include "Utils.h"
#include "pch.h"

namespace Utils
{

RE::TESForm *GetFormFromString(const std::string &str)
{
    // Expect format: 0xXXXXXX~Plugin.esp
    auto sep = str.find('~');
    if (sep == std::string::npos)
    {
        return nullptr;
    }

    std::string idPart = str.substr(0, sep);
    std::string pluginName = str.substr(sep + 1);

    // Convert hex string to formID
    RE::FormID formID = static_cast<RE::FormID>(std::stoul(idPart, nullptr, 16));

    // Look up plugin file
    auto dataHandler = RE::TESDataHandler::GetSingleton();
    if (!dataHandler)
    {
        return nullptr;
    }

    auto file = dataHandler->LookupModByName(pluginName);
    if (!file)
    {
        return nullptr;
    }

    // Compute full FormID with correct load index
    RE::FormID actualFormID = formID;
    if (file->IsLight())
    {
        // ESL (0xFE)
        actualFormID = (0xFE000000 | (file->GetPartialIndex() << 12) | (formID & 0xFFF));
    }
    else
    {
        // Standard plugin
        actualFormID |= (file->GetCompileIndex() << 24);
    }

    return RE::TESForm::LookupByID(actualFormID);
}

const RE::TESFile *GetMasterFile(RE::TESForm *ref)
{
    if (!ref)
        return nullptr;

    uint32_t formID = ref->GetFormID();
    uint8_t modIndex = static_cast<uint8_t>(formID >> 24);

    auto dataHandler = RE::TESDataHandler::GetSingleton();
    if (!dataHandler)
        return nullptr;

    if (modIndex == 0xFE)
    {
        uint16_t eslIndex = (formID >> 12) & 0xFFF;
        return dataHandler->LookupLoadedLightModByIndex(eslIndex);
    }

    return dataHandler->LookupLoadedModByIndex(modIndex);
}

std::string NormalizeFormID(RE::TESForm *form)
{
    if (!form)
    {
        return {};
    }

    auto file = Utils::GetMasterFile(form);
    if (!file)
    {
        return {};
    }

    RE::FormID formID = form->GetFormID();

    uint8_t modIndex = (formID >> 24) & 0xFF;
    uint32_t localID = formID & 0x00FFFFFF;

    if (modIndex == 0xFF)
    {
        // Dynamic form -> skip
        return {};
    }

    std::string hexPart;

    if (modIndex == 0xFE)
    {
        // ESL/light plugin -> only last 3 hex digits
        uint32_t eslID = localID & 0xFFF;
        hexPart = std::format("0x{:03X}", eslID);
    }
    else
    {
        // Normal plugin -> 6 hex digits
        hexPart = std::format("0x{:06X}", localID);
    }

    // Attach plugin filename
    return std::format("{}~{}", hexPart, file->GetFilename());
}

} // namespace Utils
