#pragma once

#include "SKSEMenuFramework.h"
#include "Settings.h"

class UI
{
  public:
    static void Register();
    static void HelpMarker(const char *a_desc);
    static void __stdcall RenderMenu();
};
