#pragma once

#define NOMINMAX

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#include <filesystem>
#include <format>
#include <iomanip>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace logger = SKSE::log;
using namespace nlohmann::literals;
using json = nlohmann::json;
