#pragma once

#include "ollama.hpp"
#include "openai.hpp"

class Config;

enum class LLMProvider {
    OpenAI,
    Ollama
};

/**
 * openai-cpp 또는 ollama-hpp를 사용하여 LLM 상호작용을 처리합니다.
 */
class LLMClient {
public:
    explicit LLMClient(const Config& config);
    ~LLMClient();

    bool TestConnection();
    void SetApiKey(const std::string& key);
    std::string SendMessage(const nlohmann::json& messages);

private:
    std::string model_;
    LLMProvider provider_;
};
