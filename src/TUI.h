#pragma once

#include <string>
#include <vector>

class TUI {
public:
    TUI();
    ~TUI();

    // Menu System
    enum class MenuOption {
        NewGame,
        LoadGame,
        Exit,
        None
    };

    MenuOption ShowMainMenu();

    // Chat Interface
    void ShowChatScreen(const std::string& characterName);
    void PrintSystem(const std::string& text);
    void PrintNpc(const std::string& name, const std::string& text);
    void PrintPlayer(const std::string& text);
    std::string ReadInput(const std::string& prompt);
    
    // Streaming support
    void PrintChunk(const std::string& chunk);
    void NewLine();

    // Helpers
    void ClearScreen();
    void WaitForKey();

private:
    void RenderMenu(int selectedIndex);
};
