#include "TUI.h"

#include <conio.h> // Windows에서 _getch 사용
#include <iostream>
#include <cstdlib>
#include <windows.h> // 콘솔 색상/코드페이지 설정용
#include <thread>
#include <chrono>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

TUI::TUI() {
    // UTF-8 출력 강제
    SetConsoleOutputCP(CP_UTF8);

    // 콘솔 글꼴과 크기 설정
    SetupConsole();
    
    // 메뉴 화면에서는 커서를 숨김(미관용)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

TUI::~TUI() {
    // 커서 표시 상태 복구
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
        if (c == 224) { // 방향키 입력을 알리는 프리픽스
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
    
    // 채팅 화면에서는 커서를 다시 보이게 함
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

void TUI::PrintNpcTyped(const std::string& name, const std::string& text) {
    NewLine();
    PrintChunk("[" + name + "] ");
    PrintChunk(text);
    NewLine();
}



void TUI::ShowEvent(const std::string& title, const std::vector<std::string>& lines) {
    ClearScreen();
    PrintSystem(">>> EVENT: " + title + " <<<");
    NewLine();
    
    for (const auto& line : lines) {
        PrintChunk(line);
        NewLine();
        WaitForKey(); 
    }
    
    PrintSystem(">>> 이벤트 종료 (Enter) <<<");
    WaitForKey();
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
    int ch;
    while ((ch = _getch()) != KEY_ENTER) {
        if (ch == 3) { // Ctrl+C 입력 시
            std::cout << "^C\n";
            // 종료 전 커서 표시 상태 복구
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(hConsole, &cursorInfo);
            cursorInfo.bVisible = TRUE;
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            exit(0);
        }
        if (ch == 0 || ch == 224) { 
            _getch();
            continue;
        }

        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else if (ch >= 32 && ch != 127) {
            password += static_cast<char>(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

std::string TUI::ShowApiKeyPrompt() {
    ClearScreen();
    std::cout << "================================================\n";
    std::cout << "             [ API 설정 필요 ]\n";
    std::cout << "================================================\n";
    std::cout << "OpenAI API 키가 설정되지 않았습니다.\n";
    std::cout << "환경 변수 OPENAI_API_KEY를 설정하거나 직접 입력하세요.\n";
    std::cout << "(입력한 키는 저장되지 않고 이번 실행에만 사용됩니다)\n\n";
    
    return ReadPassword("API Key 입력> ");
}

void TUI::ShowIntro() {
    ClearScreen();
    PrintSystem("================================================");
    PrintSystem("         [ 에피소드 : 소꿉친구의 비밀 ]");
    PrintSystem("================================================");
    PrintSystem("        어릴 적부터 함께 자란 소꿉친구.");
    PrintSystem("  오랜만에 만났는데 태도가 영 심상치 않다.");
    PrintSystem("     \"흥, 딱히 너를 기다린 건 아니거든!\"");
    PrintSystem("================================================");
    WaitForKey();
}

std::pair<std::string, std::string> TUI::ShowSetupScreen() {
    ShowChatScreen("설정");
    PrintSystem("플레이어의 이름을 입력해 주세요.");
    std::string pName = ReadInput("이름> ");
    if (pName.empty()) pName = "당신";

    PrintSystem("캐릭터 이름을 입력해 주세요.");
    std::string cName = ReadInput("이름> ");
    
    return {pName, cName};
}

std::string TUI::GetPlayerInput(const std::string& playerName) {
    return ReadInput(playerName + "> ");
}

void TUI::PrintChunk(const std::string& chunk) {
    for (size_t i = 0; i < chunk.length(); ) {
        unsigned char c = static_cast<unsigned char>(chunk[i]);
        int charLen = 1;

        if ((c & 0x80) == 0) charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        
        // 완전한 한 글자(1~4바이트)를 출력
        for (int k = 0; k < charLen && i + k < chunk.length(); ++k) {
            std::cout << chunk[i + k];
        }
        std::cout.flush();
        
        i += charLen;
        
        // 타이핑 효과를 위한 지연
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        // 공백은 지연을 건너뛰어 자연스럽게 만듦
        if (c == ' ') std::this_thread::sleep_for(std::chrono::milliseconds(0));
    }
}

void TUI::NewLine() {
    std::cout << std::endl;
}

void TUI::WaitForKey() {
    _getch();
}

void TUI::SetupConsole() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    // 1. 글꼴 설정
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    GetCurrentConsoleFontEx(hOut, FALSE, &cfi);
    
    cfi.dwFontSize.Y = 24; // 더 큰 글꼴 높이(24px)
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas"); // 가독성 좋은 모노스페이스 Consolas 사용
    SetCurrentConsoleFontEx(hOut, FALSE, &cfi);

    // 2. 콘솔 창/버퍼 크기 설정
    // 충분한 스크롤백 버퍼 확보
    COORD bufferSize = {120, 3000}; // 폭 120컬럼, 스크롤백 3000줄
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // 실제 표시되는 창 크기(viewport) 설정
    SMALL_RECT windowSize = {0, 0, 119, 39}; // 가로 120 x 세로 40 영역
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    
    // 선택 사항: 콘솔 제목 설정
    SetConsoleTitleW(L"AI 연애 시뮬레이터 - 봄날의 추억");
}
