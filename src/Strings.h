#pragma once

#include <unordered_map>
#include <string>

class Strings
{
  public:
    static void Load();
    static const char* Get(const std::string& key);

  private:
    Strings() = default;
    Strings(const Strings&) = delete;
    Strings(Strings&&) = delete;
    Strings& operator=(const Strings&) = delete;
    Strings& operator=(Strings&&) = delete;

    static inline std::unordered_map<std::string, std::string> stringMap{};
};
