#include "Settings.h"
#include "Strings.h"
#include "UI.h"
#include "logger.h"
#include "pch.h"
#include "Tasker.hpp"

void OnMessage(SKSE::MessagingInterface::Message* message)
{
    switch (message->type)
    {
    case SKSE::MessagingInterface::kDataLoaded:
        Strings::Load();
        UI::Register();
        Settings::Load();
        break;
    case SKSE::MessagingInterface::kNewGame:
    case SKSE::MessagingInterface::kPostLoadGame:
        if (Settings::delay > 0)
        {
            clib_utilsQTR::Tasker::GetSingleton()->PushTask([] { Settings::Load(); }, Settings::delay * 1000);
        }
        break;
    default:
        break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    return true;
}
