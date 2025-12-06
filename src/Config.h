#pragma once

#include <string>
#include <cstdlib>

#include "JsonHelper.h"

/**
 * JSON 파일에서 런타임 설정을 로드하고 저장하는 클래스입니다.
 */
class Config {
public:
    // 기본 설정값으로 초기화합니다.
    Config()
        : model_("gpt-5"),
          historyLimit_(5),
          charactersDir_("data/characters"),
          eventsFile_("data/events/template_events.json"),
          savesDir_("saves"),
          defaultInitialAffection_(10) {}

    // 지정된 JSON 파일에서 설정을 로드합니다.
    bool Load(const std::string& path) {
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

        assign_string("model", model_);
        
        // 보안을 위해 API 키는 JSON 파일에서 로드하지 않습니다.
        
        // 환경 변수에서 로드를 시도합니다.
        const char* envKey = std::getenv("OPENAI_API_KEY");
        if (envKey) {
            apiKey_ = envKey;
        }

        assign_int("historyLimit", historyLimit_);
        assign_string("charactersDir", charactersDir_);
        assign_string("eventsFile", eventsFile_);
        assign_string("savesDir", savesDir_);
        assign_int("defaultInitialAffection", defaultInitialAffection_);
        return true;
    }

    // 런타임에 API 키를 설정합니다.
    void SetApiKey(const std::string& key) { apiKey_ = key; }

    // 모델 식별자를 반환합니다.
    const std::string& GetModel() const { return model_; }

    // 대화 히스토리 제한(턴 수)을 반환합니다.
    int GetHistoryLimit() const { return historyLimit_; }

    // 캐릭터 데이터의 기본 디렉토리를 반환합니다.
    const std::string& GetCharactersDir() const { return charactersDir_; }

    // 이벤트 정의 파일 경로를 반환합니다.
    const std::string& GetEventsFile() const { return eventsFile_; }

    // 세이브 디렉토리를 반환합니다.
    const std::string& GetSavesDir() const { return savesDir_; }

    // 캐릭터별 설정이 없을 경우 사용할 기본 초기 호감도를 반환합니다.
    int GetDefaultInitialAffection() const { return defaultInitialAffection_; }

    // LLM 서비스용 API 키를 반환합니다.
    const std::string& GetApiKey() const { return apiKey_; }

private:
    std::string model_;
    std::string apiKey_;
    int historyLimit_;

    std::string charactersDir_;
    std::string eventsFile_;
    std::string savesDir_;
    int defaultInitialAffection_;
};
