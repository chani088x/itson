#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class TUI;
class LLMClient;
class Config;
class Character;

/**
 * 히스토리 관리를 위한 단일 대화 턴(Turn)입니다.
 */
struct DialogueTurn {
    std::string speaker;
    std::string text;
};

/**
 * 프롬프트 및 저장을 위한 대화 기록을 관리합니다.
 */
struct DialogueContext {
    // 히스토리에 새로운 발화 턴을 추가합니다.
    void AddTurn(const std::string& speaker, const std::string& text);
    
    // 기록된 모든 턴을 지웁니다.
    void Clear();

    // 전체 턴 리스트를 반환합니다.
    const std::vector<DialogueTurn>& History() const;

private:
    std::vector<DialogueTurn> history_;
};

/**
 * LLM 요청과 콘솔 출력을 중재하는 관리자 클래스입니다.
 */
class DialogueManager {
public:
    DialogueManager(TUI& ui, const Config& config);

    DialogueContext& GetContext();
    const DialogueContext& GetContext() const;

    // 사용자 입력을 기반으로 호감도 변화량을 계산합니다.
    int ScoreAffectionDelta(const std::string& userText, const std::string& npcText) const;

    // LLM 전송용 전체 JSON 페이로드(시스템 + 히스토리 + 사용자 입력)를 생성합니다.
    nlohmann::json BuildFullPrompt(Character* character, const std::string& playerName, const std::string& userInput);
    
    // LLM으로부터 응답을 받아 출력하고 반환합니다.
    std::string PrintReply(LLMClient& client, const nlohmann::json& messages, const std::string& characterName);

private:
    TUI& ui_;
    const Config& config_;
    DialogueContext context_;
};
