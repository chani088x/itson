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
    src/DialogueManager.cpp
    src/LLMClient.cpp
    src/SaveSystem.cpp
    src/TUI.cpp
)

# Always sync data directory on build
add_custom_target(SyncData ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/data
    COMMENT "Syncing data directory..."
)

add_dependencies(AIDatingSim SyncData)

target_include_directories(AIDatingSim PRIVATE src)
target_link_libraries(AIDatingSim PRIVATE CURL::libcurl nlohmann_json::nlohmann_json ws2_32 crypt32)
