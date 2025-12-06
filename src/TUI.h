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
    
    // 스트리밍 지원
    void PrintChunk(const std::string& chunk);
    void NewLine();

    // 헬퍼 함수
    void ClearScreen();
    void WaitForKey();

private:
    void RenderMenu(int selectedIndex);
};
