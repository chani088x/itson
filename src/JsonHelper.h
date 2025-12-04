#pragma once

#include <string>

#include <nlohmann/json.hpp>

/**
 * Provides thin helpers for reading/writing JSON files.
 */
class JsonHelper {
public:
    // Loads JSON from disk and stores it in `out`.
    static bool LoadFromFile(const std::string& path, nlohmann::json& out);

    // Saves JSON data to the specified file path.
    static bool SaveToFile(const std::string& path, const nlohmann::json& data);
};
