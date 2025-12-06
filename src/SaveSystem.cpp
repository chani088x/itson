#include "SaveSystem.h"

#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "Character.h"
#include "DialogueManager.h"
#include "JsonHelper.h"

namespace {
    nlohmann::json Serialize(const Character& character, const DialogueContext& context) {
        nlohmann::json data;
        data["name"] = character.GetName();
        data["affection"] = character.GetAffection();
        data["relationshipStage"] = character.GetRelationshipStage();
        data["traits"] = character.GetTraits();
        data["memories"] = character.GetMemories();

        nlohmann::json flags = nlohmann::json::object();
        for (const auto& [flag, value] : character.GetFlags()) {
            flags[flag] = value;
        }
        data["flags"] = flags;

        nlohmann::json triggered = nlohmann::json::object();
        for (const auto& [threshold, fired] : character.GetTriggeredEvents()) {
            triggered[std::to_string(threshold)] = fired;
        }
        data["triggered"] = triggered;

        nlohmann::json history = nlohmann::json::array();
        for (const auto& turn : context.History()) {
            history.push_back({{"speaker", turn.speaker}, {"text", turn.text}});
        }
        data["history"] = history;
        return data;
    }

    void Deserialize(const nlohmann::json& data, Character& character, DialogueContext& context) {
        character.SetName(data.value("name", character.GetName()));
        character.SetAffection(data.value("affection", character.GetAffection()));
        character.SetRelationshipStage(data.value("relationshipStage", character.GetRelationshipStage()));
        if (data.contains("traits") && data["traits"].is_array()) {
            character.SetTraits(data["traits"].get<std::vector<std::string>>());
        }
        if (data.contains("memories") && data["memories"].is_array()) {
            character.SetMemories(data["memories"].get<std::vector<std::string>>());
        }
        if (data.contains("flags") && data["flags"].is_object()) {
            for (auto& [key, val] : data["flags"].items()) {
                character.SetFlag(key, val.get<bool>());
            }
        }
        if (data.contains("triggered") && data["triggered"].is_object()) {
            auto& triggeredMap = character.EditableTriggeredEvents();
            triggeredMap.clear();
            for (auto& [key, val] : data["triggered"].items()) {
                triggeredMap[std::stoi(key)] = val.get<bool>();
            }
        }

        context.Clear();
        if (data.contains("history") && data["history"].is_array()) {
            for (const auto& entry : data["history"]) {
                context.AddTurn(entry.value("speaker", "Unknown"), entry.value("text", ""));
            }
        }
    }
}

SaveSystem::SaveSystem(std::string directory) : directory_(std::move(directory)) {}

std::vector<std::string> SaveSystem::ListSaveFiles() const {
    namespace fs = std::filesystem;
    std::vector<std::string> files;
    if (!fs::exists(directory_)) {
        return files;
    }
    for (const auto& entry : fs::directory_iterator(directory_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            files.push_back(entry.path().filename().string());
        }
    }
    std::sort(files.begin(), files.end(), std::greater<>());
    return files;
}

std::string SaveSystem::SaveNew(const Character& character, const DialogueContext& context) const {
    if (character.GetName().empty()) return {};

    namespace fs = std::filesystem;
    fs::create_directories(directory_);

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ts;
    ts << std::put_time(&tm, "%Y%m%d_%H%M%S");
    std::string fname = ts.str() + ".json";
    fs::path path = fs::path(directory_) / fname;

    nlohmann::json data = Serialize(character, context);
    if (!JsonHelper::SaveToFile(path.string(), data)) {
        return {};
    }
    return fname;
}

bool SaveSystem::LoadFromFile(const std::string& filename, Character& character, DialogueContext& context) const {
    nlohmann::json data;
    if (!JsonHelper::LoadFromFile(BuildPathFromName(filename), data)) {
        return false;
    }
    Deserialize(data, character, context);
    return true;
}

std::string SaveSystem::BuildPathFromName(const std::string& name) const {
    namespace fs = std::filesystem;
    fs::path path = fs::path(directory_) / name;
    return path.string();
}
