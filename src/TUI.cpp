#include "TUI.h"

#include <conio.h> // For _getch on Windows
#include <iostream>
#include <windows.h> // For console colors/CP
#include <thread>
#include <chrono>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

TUI::TUI() {
    // Ensure UTF-8 output
    SetConsoleOutputCP(CP_UTF8);
    
    // Hide cursor for menu (optional, but looks better)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

TUI::~TUI() {
    // Restore cursor
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void TUI::ClearScreen() {
    system("cls");
}

void TUI::RenderMenu(int selectedIndex) {
    ClearScreen();
    std::cout << "\n================================================\n\n";
    std::cout << "\t\t   봄날의 추억\n\n";
    std::cout << "================================================\n\n";

    const std::vector<std::string> options = {"새 플레이", "불러오기", " 나가기"};
    
    for (int i = 0; i < 3; ++i) {
        if (i == selectedIndex) {
            std::cout << "\t\t  > " << options[i] << " <\n";
        } else {
            std::cout << "\t\t    " << options[i] << "\n";
        }
    }

    std::cout << "\n================================================\n";
    std::cout << "저장: /save, 종료: /quit, 재시작: /restart\n";
    std::cout << "================================================\n";
}

TUI::MenuOption TUI::ShowMainMenu() {
    int selected = 0;
    while (true) {
        RenderMenu(selected);
        
        int c = _getch();
        if (c == 224) { // Arrow key prefix
            c = _getch();
            if (c == KEY_UP) {
                selected = (selected - 1 + 3) % 3;
            } else if (c == KEY_DOWN) {
                selected = (selected + 1) % 3;
            }
        } else if (c == KEY_ENTER) {
            if (selected == 0) return MenuOption::NewGame;
            if (selected == 1) return MenuOption::LoadGame;
            if (selected == 2) return MenuOption::Exit;
        }
    }
}

void TUI::ShowChatScreen(const std::string& characterName) {
    ClearScreen();
    std::cout << "================================================\n";
    std::cout << "대화 상대: " << characterName << "\n";
    std::cout << "================================================\n\n";
    
    // Show cursor for chat
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void TUI::PrintSystem(const std::string& text) {
    std::cout << "\n" << text << "\n";
}

void TUI::PrintNpc(const std::string& name, const std::string& text) {
    std::cout << "\n[" << name << "] " << text << "\n";
}

void TUI::PrintPlayer(const std::string& text) {
    if (text.empty()) return;
    std::cout << "\n[Player]: " << text << "\n";
}

std::string TUI::ReadInput(const std::string& prompt) {
    std::cout << "\n" << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

std::string TUI::ReadPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
    char ch;
    while ((ch = _getch()) != KEY_ENTER) {
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b"; // Erase character from screen
            }
        } else {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

void TUI::PrintChunk(const std::string& chunk) {
    for (size_t i = 0; i < chunk.length(); ) {
        unsigned char c = static_cast<unsigned char>(chunk[i]);
        int charLen = 1;

        if ((c & 0x80) == 0) charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        
        // Output the full character (1~4 bytes)
        for (int k = 0; k < charLen && i + k < chunk.length(); ++k) {
            std::cout << chunk[i + k];
        }
        std::cout.flush();
        
        i += charLen;
        
        // Delay for typing effect
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        // Skip delay for spaces to make it feel natural
        if (c == ' ') std::this_thread::sleep_for(std::chrono::milliseconds(0));
    }
}

void TUI::NewLine() {
    std::cout << std::endl;
}

void TUI::WaitForKey() {
    _getch();
}
