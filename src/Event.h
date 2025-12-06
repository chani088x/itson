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
    static Event FromJson(const nlohmann::json& data) {
        Event event;
        event.id = data.value("id", "");
        event.threshold = data.value("threshold", 0);
        event.title = data.value("title", "");
        if (data.contains("lines") && data["lines"].is_array()) {
            for (const auto& line : data["lines"]) {
                event.lines.push_back(line.get<std::string>());
            }
        }
        return event;
    }

    // 영구 저장을 위해 이 이벤트를 다시 JSON으로 변환합니다.
    nlohmann::json ToJson() const {
        nlohmann::json json;
        json["id"] = id;
        json["threshold"] = threshold;
        json["title"] = title;
        json["lines"] = lines;
        return json;
    }
};
