#include <iostream>

#ifdef _WIN32
#include <winsock2.h> // winsock2는 windows.h보다 먼저 포함해야 경고가 없음
#include <windows.h>
#endif

#include "Config.h"
#include "DialogueManager.h"
#include "Game.h"
#include "LLMClient.h"
#include "SaveSystem.h"
#include "TUI.h"

int main(int argc, char** argv) {
#ifdef _WIN32
    // Windows 터미널에서 UTF-8 출력을 활성화합니다.
    SetConsoleOutputCP(CP_UTF8);
#endif

    (void)argc;
    (void)argv;

    Config config;
    if (!config.Load("data/system/config.json")) {
        std::cerr << "설정 파일을 불러오지 못했습니다. data/system/config.json을 확인하세요.\n";
        return 1;
    }

    TUI ui;

    DialogueManager dialogueManager(config);
    LLMClient llmClient(config);
    SaveSystem saveSystem(config.GetSavesDir());

    Game game(config, ui, dialogueManager, llmClient, saveSystem);
    game.Run();
    return 0;
}
