#pragma once

#include <string>
#include <vector>

#include <nlohmann/json_fwd.hpp>

/**
 * Represents a single selectable choice inside an event.
 */
struct EventChoice {
    std::string text;
    int affectionDelta = 0;
    std::string flagSet;
};

/**
 * Represents an affection-threshold event definition.
 */
struct Event {
    std::string id;
    int threshold = 0;
    std::string title;
    std::vector<std::string> lines;
    std::vector<EventChoice> choices;

    // Builds an Event from JSON data loaded at runtime.
    static Event FromJson(const nlohmann::json& data);

    // Converts this event back into JSON for persistence.
    nlohmann::json ToJson() const;
};
