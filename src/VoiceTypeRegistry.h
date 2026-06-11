#pragma once

struct VoiceTypeEntry
{
    std::string editorID;
    std::string editorIDLower;
    std::string displayName;
    std::string displayNameLower;
    RE::FormID formID{0};
};

class VoiceTypeRegistry
{
  public:
    static VoiceTypeRegistry &GetSingleton();

    void BuildCache();
    [[nodiscard]] const std::vector<VoiceTypeEntry> &GetEntries() const;
    [[nodiscard]] std::vector<std::size_t> Filter(std::string_view a_query) const;
    [[nodiscard]] const VoiceTypeEntry *FindByEditorID(std::string_view a_editorID) const;

  private:
    VoiceTypeRegistry() = default;

    static std::string ToLower(std::string_view a_value);

    std::vector<VoiceTypeEntry> _entries;
    bool _built{false};
};
