#pragma once

#include "SKSEMenuFramework.h"

class UI
{
  public:
    static void Register();
    static void HelpMarker(const char* text);
    static void __stdcall RenderMenu();

  private:
    UI() = default;
    UI(const UI&) = delete;
    UI(UI&&) = delete;
    UI& operator=(const UI&) = delete;
    UI& operator=(UI&&) = delete;

    static void PrintHelpMarker(const char* text);
    static void BuildCache();

    inline static std::vector<std::string> voiceTypeList{};
    inline static const std::string helpMarker = FontAwesome::UnicodeToUtf8(0xf059);
};
