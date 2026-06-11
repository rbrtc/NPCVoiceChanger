#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#include <cstring>
#include <filesystem>
#include <format>
#include <iomanip>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace logger = SKSE::log;
using namespace std::literals;
using namespace nlohmann::literals;
using json = nlohmann::json;
