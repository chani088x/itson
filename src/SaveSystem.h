#pragma once

#include <string>
#include <vector>

class Character;
struct DialogueContext;

/**
 * Serializes and restores game state to JSON saves.
 */
class SaveSystem {
public:
    // Creates a save system rooted at the given directory.
    explicit SaveSystem(std::string directory);

    // Saves character state and dialogue history to disk.
    bool Save(const Character& character, const DialogueContext& context) const;

    // Loads character state and dialogue history if a save exists.
    bool Load(Character& character, DialogueContext& context) const;

    // Returns true if a save file exists for the given character.
    bool Exists(const Character& character) const;

    // Lists available save files (filenames only).
    std::vector<std::string> ListSaveFiles() const;

    // Saves to a new file with timestamp; returns filename or empty on failure.
    std::string SaveNew(const Character& character, const DialogueContext& context) const;

    // Loads from a given filename (basename only, stored in directory_).
    bool LoadFromFile(const std::string& filename, Character& character, DialogueContext& context) const;

private:
    std::string BuildPath(const Character& character) const;
    std::string BuildPathFromName(const std::string& name) const;

    std::string directory_;
};
