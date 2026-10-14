#pragma once

#include <map>
#include <string>
#include "SKSEMenuFramework.h"

class Settings
{
  public:
    static void Load();
    static void AddNPC(const std::string& npcID, const std::string& newVoiceType, const std::string& oldVoiceType,
                       const std::string& name);
    static void RemoveNPC(const std::string& npcID);
    static std::string GetNPCName(const std::string& npcID);
    static const std::map<std::string, std::string>& GetNPCList() { return savedNPCList; }
    static bool IsJsonValid(const std::string& fileName);
    static void UpdateDelay();

    static inline ImGuiMCP::ImU8 delay{};

  private:
    Settings() = default;
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings& operator=(Settings&&) = delete;

    static inline const std::string settingsFile = "Data/SKSE/Plugins/NPCVoiceChanger.json";
    // key: npcID, value: newVoiceType
    static inline std::map<std::string, std::string> savedNPCList{};
};
