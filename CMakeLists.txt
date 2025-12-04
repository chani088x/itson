cmake_minimum_required(VERSION 3.16)
project(AIDatingSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

find_package(CURL REQUIRED)

# Try config-package first; if missing, fall back to FetchContent (header-only).
find_package(nlohmann_json CONFIG QUIET)
if (NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif ()

add_executable(
    AIDatingSim
    src/main.cpp
    src/Game.cpp
    src/Character.cpp
    src/Event.cpp
    src/DialogueManager.cpp
    src/LLMClient.cpp
    src/JsonHelper.cpp
    src/SaveSystem.cpp
    src/Config.cpp
    src/Memory.cpp
    src/TUI.cpp
)

target_include_directories(AIDatingSim PRIVATE src)
target_link_libraries(AIDatingSim PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
