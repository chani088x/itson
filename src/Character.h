#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Memory.h"

/**
 * Represents a romance target with affection, relationship, and event flags.
 */
class Character {
public:
    // Creates an empty character with default stats.
    Character();

    // Creates a character initialized with the given name.
    explicit Character(std::string name);

    // Returns the character's display name.
    const std::string& GetName() const;

    // Updates the display name.
    void SetName(const std::string& name);

    // Returns the current affection value (0-100).
    int GetAffection() const;

    // Sets the affection value clamped to [0, 100].
    void SetAffection(int value);

    // Adds a delta to affection while clamping to [0, 100].
    void AddAffection(int delta);

    // Returns the relationship stage (0-3).
    int GetRelationshipStage() const;

    // Sets the relationship stage clamped to [0, 3].
    void SetRelationshipStage(int stage);

    // Advances the relationship stage by one step if possible.
    void AdvanceRelationshipStage();

    // Returns immutable personality traits.
    const std::vector<std::string>& GetTraits() const;

    // Assigns personality traits.
    void SetTraits(const std::vector<std::string>& traits);

    // Returns stored memory snippets.
    const std::vector<std::string>& GetMemories() const;

    // Adds a new memory snippet.
    void AddMemory(const std::string& memory);

    // Replaces all memories with the provided list.
    void SetMemories(const std::vector<std::string>& memories);

    // Clears every stored memory.
    void ClearMemories();

    // Sets or clears an arbitrary event flag.
    void SetFlag(const std::string& flag, bool value);

    // Returns the value of an event flag.
    bool GetFlag(const std::string& flag) const;

    // Returns all event flags for serialization.
    const std::unordered_map<std::string, bool>& GetFlags() const;

    // Marks a threshold-based event as fired.
    void MarkEventTriggered(int threshold);

    // Returns whether a given threshold event already fired.
    bool HasTriggeredEvent(int threshold) const;

    // Returns all triggered thresholds for persistence.
    const std::unordered_map<int, bool>& GetTriggeredEvents() const;

    // Returns mutable triggered map for loading saves.
    std::unordered_map<int, bool>& EditableTriggeredEvents();

private:
    std::string name_;
    int affection_;
    int relationshipStage_;
    std::vector<std::string> traits_;
    Memory memories_;
    std::unordered_map<std::string, bool> flags_;
    std::unordered_map<int, bool> triggeredEvents_;
};
