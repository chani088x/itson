#pragma once

#include <string>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

/**
 * JSON 파일을 읽고 쓰기 위한 간단한 헬퍼 클래스입니다.
 */
class JsonHelper {
public:
    // 디스크에서 JSON을 로드하여 `out` 변수에 저장합니다.
    static inline bool LoadFromFile(const std::string& path, nlohmann::json& out) {
        std::ifstream input(path);
        if (!input.is_open()) {
            std::cerr << "[JsonHelper] 파일을 열 수 없습니다: " << path << '\n';
            return false;
        }
        try {
            input >> out;
        } catch (const std::exception& e) {
            std::cerr << "[JsonHelper] JSON 파싱 오류(" << path << "): " << e.what() << '\n';
            return false;
        }
        return true;
    }

    // JSON 데이터를 지정된 파일 경로에 저장합니다.
    static inline bool SaveToFile(const std::string& path, const nlohmann::json& data) {
        std::ofstream output(path);
        if (!output.is_open()) {
            std::cerr << "[JsonHelper] 파일을 쓸 수 없습니다: " << path << '\n';
            return false;
        }
        output << data.dump(2);
        return true;
    }
};
