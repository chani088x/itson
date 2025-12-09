#pragma once

#include <string>
#include <vector>
#include "Character.h"
#include "Event.h"
#include "TUI.h"

class Config;
class DialogueManager;
class LLMClient;
class SaveSystem;

/**
 * 게임의 메인 루프, 입력 처리, 이벤트 관리 등을 담당하는 핵심 클래스입니다.
 */
class Game {
public:
    Game(Config& config,
         TUI& ui,
         DialogueManager& dialogueManager,
         LLMClient& llmClient,
         SaveSystem& saveSystem);

    void Run();

private:
    void ProcessTurn(const std::string& userInput);
    bool HandleMetaCommand(const std::string& input);
    void SaveProgress();
    void LoadProgress();
    void AutoAdvanceRelationship(Character& character);
    void RunGameLoop();
    void PromptLoadSelection();
    
    // 이벤트 시스템
    void LoadEvents(const std::string& filePath);
    void CheckAndTriggerEvents();
    void PlayEvent(const Event& event);
    void RestoreChatHistory();

    Config& config_;
    TUI& ui_;
    DialogueManager& dialogueManager_;
    LLMClient& llmClient_;
    SaveSystem& saveSystem_;

    std::vector<Character> characters_;
    Character* activeCharacter_;
    std::string playerName_;
    bool isRunning_;
    
    std::vector<Event> events_;
};
