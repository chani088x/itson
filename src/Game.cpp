#include "Game.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <thread>
#include <chrono>
#include <vector>

#include "Character.h"
#include "Config.h"
#include "DialogueManager.h"
#include "JsonHelper.h"
#include "LLMClient.h"
#include "SaveSystem.h"
#include "TUI.h"

namespace {
constexpr std::array<int, 4> kStageThresholds{0, 25, 50, 80};
}  // namespace

Game::Game(Config& config,
           TUI& ui,
           DialogueManager& dialogueManager,
           LLMClient& llmClient,
           SaveSystem& saveSystem)
    : config_(config),
      ui_(ui),
      dialogueManager_(dialogueManager),
      llmClient_(llmClient),
      saveSystem_(saveSystem),
      activeCharacter_(nullptr),
      isRunning_(false) {}

void Game::Run() {
    while (true) {
        TUI::MenuOption option = ui_.ShowMainMenu();
        
        if (option == TUI::MenuOption::Exit) {
            break;
        }

        if (option == TUI::MenuOption::NewGame) {
            characters_.clear();
            Character fallback("샘플 캐릭터");
            fallback.SetTraits({"온화함", "낙천주의"});
            fallback.SetAffection(config_.GetDefaultInitialAffection());
            fallback.SetRelationshipStage(0);
            characters_.push_back(fallback);
            activeCharacter_ = &characters_.front();
            
            dialogueManager_.GetContext().Clear();
            LoadEvents(config_.GetEventsFile());
            
            ui_.ShowChatScreen(activeCharacter_->GetName());
            ui_.PrintSystem("캐릭터 이름을 입력해 주세요.");
            std::string nameInput = ui_.ReadInput("이름> ");
            if (!nameInput.empty()) {
                activeCharacter_->SetName(nameInput);
                ui_.ShowChatScreen(activeCharacter_->GetName());
            }
            
            ui_.PrintSystem(activeCharacter_->GetName() + "과(와) 이야기를 시작합니다.");
            
            isRunning_ = true;
            while (isRunning_) {
                std::string input = ui_.ReadInput("당신> ");
                if (input.empty()) continue;

                if (input.front() == '/') {
                    if (HandleMetaCommand(input.substr(1))) {
                        if (!isRunning_) break;
                        continue;
                    }
                }
                ProcessTurn(input);
            }
        } else if (option == TUI::MenuOption::LoadGame) {
            PromptLoadSelection();
            if (activeCharacter_) {
                ui_.ShowChatScreen(activeCharacter_->GetName());
                LoadEvents(config_.GetEventsFile());
                isRunning_ = true;
                while (isRunning_) {
                    std::string input = ui_.ReadInput("당신> ");
                    if (input.empty()) continue;

                    if (input.front() == '/') {
                        if (HandleMetaCommand(input.substr(1))) {
                            if (!isRunning_) break;
                            continue;
                        }
                    }
                    ProcessTurn(input);
                }
            }
        }
    }
    ui_.PrintSystem("게임을 종료합니다.");
}

void Game::ProcessTurn(const std::string& userInput) {
    if (!activeCharacter_) return;

    DialogueContext& context = dialogueManager_.GetContext();
    context.AddTurn("Player", userInput);

    std::string prompt = "System: You are " + activeCharacter_->GetName() + ".\n";
    prompt += "Traits: ";
    for (const auto& t : activeCharacter_->GetTraits()) prompt += t + ", ";
    prompt += "\n";
    prompt += "Affection: " + std::to_string(activeCharacter_->GetAffection()) + "\n";
    prompt += "History:\n" + context.ToHistoryString(10) + "\n";
    prompt += "Player: " + userInput + "\n";
    prompt += activeCharacter_->GetName() + ":";

    std::string npcReply = config_.UseStreaming()
                                ? dialogueManager_.PrintStreaming(llmClient_, prompt)
                                : dialogueManager_.PrintNonStreaming(llmClient_, prompt);

    context.AddTurn(activeCharacter_->GetName(), npcReply);

    int affectionDelta = dialogueManager_.ScoreAffectionDelta(userInput, npcReply);
    if (affectionDelta != 0) {
        activeCharacter_->AddAffection(affectionDelta);
        AutoAdvanceRelationship(*activeCharacter_);
    }
    
    if (affectionDelta > 0) {
        ui_.PrintSystem("호감도 상승! (현재 호감도:" + std::to_string(activeCharacter_->GetAffection()) + ")");
    } else if (affectionDelta < 0) {
        ui_.PrintSystem("호감도 하락... (현재 호감도:" + std::to_string(activeCharacter_->GetAffection()) + ")");
    }

    CheckAndTriggerEvents();
}

bool Game::HandleMetaCommand(const std::string& cmd) {
    std::string lowered = cmd;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    if (lowered == "quit" || lowered == "exit") {
        isRunning_ = false;
        return true;
    }
    if (lowered == "save") {
        SaveProgress();
        return true;
    }
    if (lowered == "restart") {
        isRunning_ = false;
        return true;
    }
    if (lowered == "help") {
        ui_.PrintSystem("/save, /quit, /restart");
        return true;
    }
    return false;
}

void Game::SaveProgress() {
    if (!activeCharacter_) return;
    std::string fname = saveSystem_.SaveNew(*activeCharacter_, dialogueManager_.GetContext());
    if (!fname.empty()) ui_.PrintSystem("저장 완료: " + fname);
    else ui_.PrintSystem("저장 실패");
}

void Game::LoadProgress() {
    // Logic moved to Run() / Menu
}

void Game::AutoAdvanceRelationship(Character& character) {
    int currentStage = character.GetRelationshipStage();
    if (currentStage >= static_cast<int>(kStageThresholds.size()) - 1) return;

    int nextStage = currentStage + 1;
    if (character.GetAffection() >= kStageThresholds[nextStage]) {
        character.AdvanceRelationshipStage();
        ui_.PrintSystem("관계 단계 상승! (" + std::to_string(nextStage) + ")");
    }
}

void Game::ReturnToStartMenu() {
    isRunning_ = false;
}

void Game::UpdateStatusPanel() {
}

void Game::PromptLoadSelection() {
    auto files = saveSystem_.ListSaveFiles();
    if (files.empty()) {
        ui_.PrintSystem("저장 파일이 없습니다.");
        ui_.WaitForKey();
        return;
    }
    
    ui_.ClearScreen();
    ui_.PrintSystem("불러올 파일을 선택하세요 (번호 입력):");
    for (size_t i = 0; i < files.size(); ++i) {
        ui_.PrintSystem(std::to_string(i + 1) + ". " + files[i]);
    }
    
    std::string input = ui_.ReadInput("선택> ");
    try {
        int idx = std::stoi(input);
        if (idx >= 1 && idx <= (int)files.size()) {
            characters_.clear();
            Character loaded("Temp");
            if (saveSystem_.LoadFromFile(files[idx - 1], loaded, dialogueManager_.GetContext())) {
                characters_.push_back(loaded);
                activeCharacter_ = &characters_.front();
                ui_.PrintSystem("로드 성공! Enter를 눌러 게임을 시작하세요!");
                ui_.WaitForKey();
            } else {
                ui_.PrintSystem("로드 실패.");
                ui_.WaitForKey();
            }
        }
    } catch (...) {
        ui_.PrintSystem("잘못된 입력입니다.");
        ui_.WaitForKey();
    }
}

void Game::LoadEvents(const std::string& filePath) {
    nlohmann::json doc;
    if (!JsonHelper::LoadFromFile(filePath, doc)) {
        return;
    }
    if (!doc.is_array()) {
        return;
    }

    events_.clear();
    for (const auto& entry : doc) {
        events_.push_back(Event::FromJson(entry));
    }
}

void Game::CheckAndTriggerEvents() {
    if (!activeCharacter_) return;
    
    for (const auto& event : events_) {
        if (activeCharacter_->GetAffection() >= event.threshold && 
            !activeCharacter_->HasTriggeredEvent(event.threshold)) {
            
            ui_.PrintSystem("이벤트를 보시겠습니까? (y/n)");
            std::string ans = ui_.ReadInput("> ");
            if (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y')) {
                PlayEvent(event);
                activeCharacter_->MarkEventTriggered(event.threshold);
            }
        }
    }
}

void Game::PlayEvent(const Event& event) {
    ui_.ClearScreen();
    ui_.PrintSystem("=== " + event.title + " ===");
    ui_.NewLine();
    
    for (const auto& line : event.lines) {
        // Stream the line char by char
        for (char c : line) {
            std::string s(1, c);
            ui_.PrintChunk(s);
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Typing effect
        }
        ui_.NewLine();
        ui_.WaitForKey(); // Wait for click/key
    }
    
    if (!event.choices.empty()) {
        ui_.NewLine();
        ui_.PrintSystem("선택지:");
        for (size_t i = 0; i < event.choices.size(); ++i) {
            ui_.PrintSystem(std::to_string(i + 1) + ". " + event.choices[i].text);
        }
        
        while (true) {
            std::string input = ui_.ReadInput("선택> ");
            try {
                int idx = std::stoi(input) - 1;
                if (idx >= 0 && idx < (int)event.choices.size()) {
                    const auto& choice = event.choices[idx];
                    activeCharacter_->AddAffection(choice.affectionDelta);
                    ui_.PrintSystem(std::string("선택 완료. (호감도 ") + 
                        (choice.affectionDelta >= 0 ? "+" : "") + 
                        std::to_string(choice.affectionDelta) + ")");
                    break;
                }
            } catch (...) {}
            ui_.PrintSystem("올바른 번호를 입력하세요.");
        }
    }
    
    ui_.WaitForKey();
    ui_.ShowChatScreen(activeCharacter_->GetName()); // Restore chat screen
}
