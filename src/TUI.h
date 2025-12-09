#pragma once

#include <string>
#include <vector>

class TUI {
public:
    TUI();
    ~TUI();

    // 메뉴 시스템
    enum class MenuOption {
        NewGame,
        LoadGame,
        Exit,
        None
    };

    MenuOption ShowMainMenu();

    // 채팅 인터페이스
    void ShowChatScreen(const std::string& characterName);
    void PrintSystem(const std::string& text);
    void PrintNpc(const std::string& name, const std::string& text);
    void PrintPlayer(const std::string& text);
    std::string ReadInput(const std::string& prompt);
    std::string ReadPassword(const std::string& prompt);
    
    // API 키 입력 화면
    std::string ShowApiKeyPrompt();

    // 인트로 화면 출력
    void ShowIntro();

    // 초기 설정 화면 (플레이어 이름, 캐릭터 이름 입력)
    // first: playerName, second: characterName (empty allowed)
    std::pair<std::string, std::string> ShowSetupScreen();

    // 메인 게임 루프에서 플레이어 입력 받기
    std::string GetPlayerInput(const std::string& playerName);
    
    // 텍스트 출력 (타이핑 효과)
    void PrintChunk(const std::string& chunk);
    void NewLine();

    // 헬퍼 함수
    void ClearScreen();
    void WaitForKey();

private:
    void RenderMenu(int selectedIndex);
    void SetupConsole();
};
