#pragma once

#include <string>
#include <vector>

class Character;
struct DialogueContext;

/**
 * 게임 상태를 JSON 형식으로 직렬화(저장)하고 복원(로드)합니다.
 */
class SaveSystem {
public:
    // 지정된 디렉토리를 루트로 하는 저장 시스템을 생성합니다.
    explicit SaveSystem(std::string directory);

    // 현재 상태를 새로운 파일로 저장합니다. (타임스탬프)
    std::string SaveNew(const Character& character, const DialogueContext& context) const;

    // 특정 파일을 로드합니다.
    bool LoadFromFile(const std::string& filename, Character& character, DialogueContext& context) const;

    // 저장된 파일 목록을 반환합니다.
    std::vector<std::string> ListSaveFiles() const;

private:
    std::string directory_;
    std::string BuildPathFromName(const std::string& name) const;
};
