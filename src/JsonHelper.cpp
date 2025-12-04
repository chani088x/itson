#include "JsonHelper.h"

#include <fstream>
#include <iostream>

bool JsonHelper::LoadFromFile(const std::string& path, nlohmann::json& out) {
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

bool JsonHelper::SaveToFile(const std::string& path, const nlohmann::json& data) {
    std::ofstream output(path);
    if (!output.is_open()) {
        std::cerr << "[JsonHelper] 파일을 쓸 수 없습니다: " << path << '\n';
        return false;
    }
    output << data.dump(2);
    return true;
}
