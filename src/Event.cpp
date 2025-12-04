#include "Event.h"

#include <nlohmann/json.hpp>

Event Event::FromJson(const nlohmann::json& data) {
    Event event;
    event.id = data.value("id", "");
    event.threshold = data.value("threshold", 0);
    event.title = data.value("title", "");
    if (data.contains("lines") && data["lines"].is_array()) {
        for (const auto& line : data["lines"]) {
            event.lines.push_back(line.get<std::string>());
        }
    }
    if (data.contains("choices") && data["choices"].is_array()) {
        for (const auto& choice : data["choices"]) {
            EventChoice option;
            option.text = choice.value("text", "");
            option.affectionDelta = choice.value("affectionDelta", 0);
            option.flagSet = choice.value("flagSet", "");
            event.choices.push_back(option);
        }
    }
    return event;
}

nlohmann::json Event::ToJson() const {
    nlohmann::json json;
    json["id"] = id;
    json["threshold"] = threshold;
    json["title"] = title;
    json["lines"] = lines;
    nlohmann::json choicesJson = nlohmann::json::array();
    for (const auto& choice : choices) {
        choicesJson.push_back({
            {"text", choice.text},
            {"affectionDelta", choice.affectionDelta},
            {"flagSet", choice.flagSet},
        });
    }
    json["choices"] = choicesJson;
    return json;
}
