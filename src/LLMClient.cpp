#include "LLMClient.h"
#include "Config.h"

#include <iostream>

LLMClient::LLMClient(const Config& config)
    : model_(config.GetModel()) {

    std::string apiKey = config.GetApiKey();
    
    // API 키 존재 여부에 따라 제공자를 결정합니다.
    // API 키가 있으면 OpenAI(클라우드)로 간주합니다.
    // API 키가 없으면 Ollama(로컬, 인증 없음)로 간주합니다.
    
    if (!apiKey.empty()) {
        provider_ = LLMProvider::OpenAI;
        openai::start(apiKey);
    } else {
        provider_ = LLMProvider::Ollama;
        // ollama-hpp는 기본값인 localhost:11434를 사용합니다. 
    }
}

LLMClient::~LLMClient() {
}

void LLMClient::SetApiKey(const std::string& key) {
    if (!key.empty()) {
        provider_ = LLMProvider::OpenAI;
        openai::start(key);
    } else {
        provider_ = LLMProvider::Ollama;
    }
}

bool LLMClient::TestConnection() {
    try {
        if (provider_ == LLMProvider::Ollama) {
            ollama::messages msgs;
            msgs.push_back(ollama::message("user", "test"));
            ollama::chat(model_, msgs);
            return true;
        } else {
            // OpenAI 테스트
            auto completion = openai::chat().create({
                {"model", "gpt-3.5-turbo"}, 
                {"messages", {{{"role", "user"}, {"content", "test"}}}}
            });
            // create가 예외를 던지거나 에러 json을 반환하면 잡아서 처리합니다.
            if (completion.contains("error")) return false;
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "[DEBUG] TestConnection Failed: " << e.what() << std::endl;
        return false;
    }
}

std::string LLMClient::SendMessage(const nlohmann::json& jsonMessages) {
    try {
        if (provider_ == LLMProvider::Ollama) {
            ollama::messages msgs;
            for (const auto& item : jsonMessages) {
                std::string role = item.value("role", "user");
                std::string content = item.value("content", "");
                msgs.push_back(ollama::message(role, content));
            }
            ollama::response response = ollama::chat(model_, msgs);
            
            // Ollama: 필드에 안전하게 접근하기 위해 응답을 json으로 변환
            nlohmann::json j = response; 
            if (j.contains("message") && j["message"].contains("content")) {
                return j["message"]["content"].get<std::string>();
            }
            return "Error: Unexpected Ollama response format";
        } else {
            // OpenAI
            nlohmann::json payload = {
                {"model", model_},
                {"messages", jsonMessages}
            };
            auto res = openai::chat().create(payload);
            
            if (res.contains("choices") && !res["choices"].empty()) {
                return res["choices"][0]["message"]["content"].get<std::string>();
            }
            if (res.contains("error")) {
                return "Error: " + res["error"]["message"].get<std::string>();
            }
            return "Error: Empty OpenAI response";
        }
    } catch (const std::exception& e) {
        return std::string("Error: ") + e.what();
    }
}
