#pragma once

#include <string>
#include <vector>

/**
 * Stores short descriptive memories for a character route.
 */
class Memory {
public:
    // Adds a new memory snippet to the log.
    void Add(const std::string& entry);

    // Replaces the stored memories with the provided list.
    void Set(const std::vector<std::string>& entries);

    // Removes all existing memories.
    void Clear();

    // Returns an immutable view of all stored memories.
    const std::vector<std::string>& Entries() const;

private:
    std::vector<std::string> entries_;
};
