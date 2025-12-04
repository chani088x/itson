#include "Config.h"

#include "JsonHelper.h"

Config::Config()
    : model_("qwen2.5:7b"),
      endpoint_("http://localhost:11434/api/generate"),
      historyLimit_(5),
      useStreaming_(true),
      typingEffect_(true),
      colorOutput_(true),
      typingDelayMs_(15),
      charactersDir_("data/characters"),
      eventsFile_("data/events/template_events.json"),
      savesDir_("saves"),
      defaultInitialAffection_(10) {}

bool Config::Load(const std::string& path) {
    nlohmann::json data;
    if (!JsonHelper::LoadFromFile(path, data)) {
        return false;
    }

    auto assign_string = [&](const char* key, std::string& target) {
        if (data.contains(key) && data[key].is_string()) {
            target = data[key].get<std::string>();
        }
    };
    auto assign_int = [&](const char* key, int& target) {
        if (data.contains(key) && data[key].is_number_integer()) {
            target = data[key].get<int>();
        }
    };
    auto assign_bool = [&](const char* key, bool& target) {
        if (data.contains(key) && data[key].is_boolean()) {
            target = data[key].get<bool>();
        }
    };

    assign_string("model", model_);
    assign_string("endpoint", endpoint_);
    assign_int("historyLimit", historyLimit_);
    assign_bool("useStreaming", useStreaming_);
    assign_bool("typingEffect", typingEffect_);
    assign_bool("colorOutput", colorOutput_);
    assign_int("typingDelayMs", typingDelayMs_);
    assign_string("charactersDir", charactersDir_);
    assign_string("eventsFile", eventsFile_);
    assign_string("savesDir", savesDir_);
    assign_int("defaultInitialAffection", defaultInitialAffection_);
    return true;
}

const std::string& Config::GetModel() const {
    return model_;
}

const std::string& Config::GetEndpoint() const {
    return endpoint_;
}

int Config::GetHistoryLimit() const {
    return historyLimit_;
}

bool Config::UseStreaming() const {
    return useStreaming_;
}

bool Config::UseTypingEffect() const {
    return typingEffect_;
}

bool Config::UseColorOutput() const {
    return colorOutput_;
}

int Config::GetTypingDelayMs() const {
    return typingDelayMs_;
}

const std::string& Config::GetCharactersDir() const {
    return charactersDir_;
}

const std::string& Config::GetEventsFile() const {
    return eventsFile_;
}

const std::string& Config::GetSavesDir() const {
    return savesDir_;
}

int Config::GetDefaultInitialAffection() const {
    return defaultInitialAffection_;
}
