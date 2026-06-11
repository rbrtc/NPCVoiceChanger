#include "Settings.h"
#include "Strings.h"
#include "UI.h"
#include "VoiceTypeRegistry.h"
#include "logger.h"
#include "pch.h"

void OnMessage(SKSE::MessagingInterface::Message *message)
{
    switch (message->type)
    {
    case SKSE::MessagingInterface::kDataLoaded:
        Strings::Load();
        VoiceTypeRegistry::GetSingleton().BuildCache();
        UI::Register();
        break;
    case SKSE::MessagingInterface::kPreLoadGame:
        break;
    case SKSE::MessagingInterface::kPostLoadGame:
        Settings::GetSingleton().Load();
        break;
    case SKSE::MessagingInterface::kNewGame:
        break;
    default:
        break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse)
{
    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    return true;
}
