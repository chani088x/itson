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

# nlohmann_json is now included locally in src/nlohmann/json.hpp

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

# Create a custom target that runs EVERY time a build is requested
add_custom_target(SyncData COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_BINARY_DIR}/data
    COMMENT "Syncing data directory..."
)
add_dependencies(AIDatingSim SyncData)

target_include_directories(AIDatingSim PRIVATE src)
target_link_libraries(AIDatingSim PRIVATE CURL::libcurl ws2_32 crypt32)
