#pragma once

class Strings
{
  public:
    static void Load();
    static const char *Get(std::string_view a_key);

  private:
    static std::unordered_map<std::string, std::string> _map;
};
