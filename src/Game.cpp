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
constexpr std::array<int, 5> kStageThresholds{0, 25, 50, 75, 100};
}  // 익명 네임스페이스 종료

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
    // 초기 API 키 검증 루프
    bool isKeyValid = false;
    while (!isKeyValid) {
        if (config_.GetApiKey().empty()) {
            std::string key = ui_.ShowApiKeyPrompt();
            if (!key.empty()) {
                config_.SetApiKey(key);
                llmClient_.SetApiKey(key);
            } else {
                ui_.PrintSystem("경고: API 키 없이 진행합니다. AI 응답이 실패할 수 있습니다.");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                break; // 사용자가 명시적으로 건너뜀
            }
        }

        ui_.PrintSystem("API 키 검증 중...");
        if (llmClient_.TestConnection()) {
            ui_.PrintSystem("검증 성공! 게임을 시작합니다.");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            isKeyValid = true;
        } else {
            ui_.PrintSystem("!!! 오류: API 키 검증에 실패했습니다. (401 Unauthorized 등) !!!");
            ui_.PrintSystem("키가 올바른지 확인해 주세요.");
            ui_.PrintSystem("Enter를 누르면 다시 입력합니다.");
            ui_.WaitForKey();
            config_.SetApiKey(""); // 프롬프트 루프를 강제하기 위해 초기화
        }
    }
    
    ui_.ClearScreen();

    while (true) {
        TUI::MenuOption option = ui_.ShowMainMenu();
        
        if (option == TUI::MenuOption::Exit) {
            break;
        }

        if (option == TUI::MenuOption::NewGame) {
            characters_.clear();
            
            nlohmann::json charData;
            Character newChar("New Character");
            if (JsonHelper::LoadFromFile("data/characters/template_character.json", charData)) {
                newChar = charData.get<Character>(); // 누락된 키는 JSON 변환 과정에서 기본값으로 처리됨
                
                // 꼭 필요하면 기본값으로 덮어쓸 수 있지만 Character.cpp의 from_json이 이미 처리함
                if (charData.contains("initialAffection") && !charData.contains("affection")) {
                     // from_json에서 처리
                }
            } else {
                ui_.PrintSystem("경고: 캐릭터 템플릿(data/characters/template_character.json)을 찾을 수 없습니다.");
                ui_.PrintSystem("기본값(샘플 캐릭터)으로 시작합니다.");
                
                newChar.SetName("샘플 캐릭터");
                newChar.SetTraits({"온화함", "낙천주의"});
                newChar.SetAffection(config_.GetDefaultInitialAffection());
                newChar.SetRelationshipStage(0);
            }

            characters_.push_back(newChar);
            activeCharacter_ = &characters_.front();
            
            dialogueManager_.GetContext().Clear();
            LoadEvents(config_.GetEventsFile());


            ui_.ShowIntro();
            
            auto [pName, cName] = ui_.ShowSetupScreen();
            playerName_ = pName;
            
            if (!cName.empty()) {
                activeCharacter_->SetName(cName);
            }
            
            ui_.ShowChatScreen(activeCharacter_->GetName());
            ui_.PrintSystem(activeCharacter_->GetName() + "과(와) 이야기를 시작합니다.");
            
            RunGameLoop();
        } else if (option == TUI::MenuOption::LoadGame) {
            PromptLoadSelection();
            if (activeCharacter_) {
                if (playerName_.empty()) playerName_ = "당신"; 
                ui_.ShowChatScreen(activeCharacter_->GetName());
                LoadEvents(config_.GetEventsFile());
                RunGameLoop();
            }
        }
    }
    ui_.PrintSystem("게임을 종료합니다.");
}

void Game::RunGameLoop() {
    isRunning_ = true;
    while (isRunning_) {
        std::string input = ui_.GetPlayerInput(playerName_);
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

void Game::ProcessTurn(const std::string& userInput) {
    if (!activeCharacter_) return;

    DialogueContext& context = dialogueManager_.GetContext();
    context.AddTurn(playerName_, userInput);

    // 채팅 메시지 생성 (DialogueManager에게 위임)
    nlohmann::json messages = dialogueManager_.BuildFullPrompt(activeCharacter_, playerName_);

    // LLM으로부터 응답 수신 (UI 출력 없음)
    std::string npcReply = dialogueManager_.FetchNpcResponse(llmClient_, messages);

    // TUI를 통해 출력 (Game 클래스가 직접 UI 제어)
    ui_.PrintNpcTyped(activeCharacter_->GetName(), npcReply);

    context.AddTurn(activeCharacter_->GetName(), npcReply);

    int affectionDelta = dialogueManager_.ScoreAffectionDelta(userInput);
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
    // 로직이 Run()이나 메뉴로 이동됨
}

void Game::AutoAdvanceRelationship(Character& character) {
    int currentStage = character.GetRelationshipStage();
    if (currentStage >= static_cast<int>(kStageThresholds.size()) - 1) return;

    // 참고: 현재 임계값은 kStageThresholds(0, 25, 50, 75, 100)에 하드코딩 되어 있습니다.
    int nextStage = currentStage + 1;
    if (character.GetAffection() >= kStageThresholds[nextStage]) {
        character.AdvanceRelationshipStage();
        ui_.PrintSystem("관계 단계 상승! (" + std::to_string(nextStage) + ")");
    }
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

    events_ = doc.get<std::vector<Event>>();
}

void Game::CheckAndTriggerEvents() {
    if (!activeCharacter_) return;
    
    bool triggered = false;
    int currentAffection = activeCharacter_->GetAffection();
    
    for (const auto& event : events_) {
        // 조건:
        // 1. 호감도 >= 임계값
        // 2. 아직 발생하지 않은 이벤트
        if (event.threshold > 0 && 
            currentAffection >= event.threshold && 
            !activeCharacter_->HasTriggeredEvent(event.threshold)) {
            
            ui_.PrintSystem(">>> 이벤트 발생 조건 달성: [" + event.title + "]");
            std::string ans = ui_.ReadInput("이벤트를 보시겠습니까? (y/n)> ");
            if (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y')) {
                // 이벤트 전 자동 저장
                ui_.PrintSystem("[시스템] 이벤트 진입 전 자동 저장을 수행합니다...");
                SaveProgress();
                
                PlayEvent(event);
                activeCharacter_->MarkEventTriggered(event.threshold);
                triggered = true;
            }
        }
    }
    
    if (triggered) {
        ui_.ClearScreen();
        ui_.ShowChatScreen(activeCharacter_->GetName());
        RestoreChatHistory(); // 채팅 내역 복구
    }
}

void Game::PlayEvent(const Event& event) {
    ui_.ShowEvent(event.title, event.lines);
}

void Game::RestoreChatHistory() {
    const auto& history = dialogueManager_.GetContext().History();
    for (const auto& turn : history) {
        if (turn.speaker == "Player" || turn.speaker == playerName_) {
            ui_.PrintPlayer(turn.text);
        } else {
            ui_.PrintNpc(turn.speaker, turn.text);
        }
    }
}
