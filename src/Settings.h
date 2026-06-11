#pragma once

class Settings
{
  public:
    static Settings &GetSingleton();

    void Load();
    void AddNPC(const std::string &npcID, const std::string &newVoiceType, const std::string &oldVoiceType,
                const std::string &name);
    void RemoveNPC(const std::string &npcID);

    const std::string GetNPCName(const std::string &npcID) const;
    const std::map<std::string, std::string> GetNPCList() const
    {
        return savedNPCList;
    }

  private:
    Settings() = default;

    const bool IsJsonValid() const;
    const std::string jsonFileName = "Data/SKSE/Plugins/NPCVoiceChanger.json";
    std::map<std::string, std::string> savedNPCList;
};
