#pragma once

namespace Utils
{
std::string NormalizeFormID(RE::TESForm *form);
RE::TESForm *GetFormFromString(const std::string &str);
const RE::TESFile *GetMasterFile(RE::TESForm *ref);
} // namespace Utils
