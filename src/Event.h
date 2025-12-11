#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

/**
 * 호감도 임계값 이벤트 정의를 나타냅니다.
 */
struct Event {
    std::string id;
    int threshold = 0;
    std::string title;
    std::vector<std::string> lines;

    // 런타임에 로드된 JSON 데이터로부터 Event 객체를 생성합니다.
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Event, id, threshold, title, lines)
};
