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
    void ReturnToStartMenu();
    void UpdateStatusPanel();
    void PromptLoadSelection();
    
    // Event System
    void LoadEvents(const std::string& filePath);
    void CheckAndTriggerEvents();
    void PlayEvent(const Event& event);

    Config& config_;
    TUI& ui_;
    DialogueManager& dialogueManager_;
    LLMClient& llmClient_;
    SaveSystem& saveSystem_;

    std::vector<Character> characters_;
    Character* activeCharacter_;
    bool isRunning_;
    
    std::vector<Event> events_;
};
