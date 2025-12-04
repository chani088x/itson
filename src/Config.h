#pragma once

#include <string>

/**
 * Stores runtime configuration loaded from JSON.
 */
class Config {
public:
    // Initializes default configuration values.
    Config();

    // Loads configuration values from the given JSON file.
    bool Load(const std::string& path);

    // Returns the Ollama model identifier.
    const std::string& GetModel() const;

    // Returns the Ollama HTTP endpoint.
    const std::string& GetEndpoint() const;

    // Returns how many turns to keep in history.
    int GetHistoryLimit() const;

    // Returns whether streaming mode should be used.
    bool UseStreaming() const;

    // Returns whether the typing effect is enabled.
    bool UseTypingEffect() const;

    // Returns whether ANSI/console colors are enabled.
    bool UseColorOutput() const;

    // Returns the typing delay per character in milliseconds.
    int GetTypingDelayMs() const;

    // Returns base directory for character data.
    const std::string& GetCharactersDir() const;

    // Returns event definition file path.
    const std::string& GetEventsFile() const;

    // Returns save directory.
    const std::string& GetSavesDir() const;

    // Returns default initial affection when not specified per character.
    int GetDefaultInitialAffection() const;

private:
    std::string model_;
    std::string endpoint_;
    int historyLimit_;
    bool useStreaming_;
    bool typingEffect_;
    bool colorOutput_;
    int typingDelayMs_;

    std::string charactersDir_;
    std::string eventsFile_;
    std::string savesDir_;
    int defaultInitialAffection_;
};
