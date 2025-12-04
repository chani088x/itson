#pragma once

#include <string>
#include <vector>

class TUI;
class LLMClient;
class Config;

/**
 * Represents a single dialogue turn for history.
 */
struct DialogueTurn {
    std::string speaker;
    std::string text;
};

/**
 * Stores rolling conversation history for prompts and saves.
 */
struct DialogueContext {
    // Adds a new speaker turn to the history.
    void AddTurn(const std::string& speaker, const std::string& text);

    // Summarizes the last N lines into a string.
    std::string ToHistoryString(std::size_t limit) const;

    // Clears every recorded turn.
    void Clear();

    // Returns the entire turn list.
    const std::vector<DialogueTurn>& History() const;

private:
    std::vector<DialogueTurn> history_;
};

/**
 * Mediates LLM requests and console presentation.
 */
class DialogueManager {
public:
    DialogueManager(TUI& ui, const Config& config);

    DialogueContext& GetContext();
    const DialogueContext& GetContext() const;

    int ScoreAffectionDelta(const std::string& userText, const std::string& npcText) const;

    std::string PrintStreaming(LLMClient& client, const std::string& prompt);
    std::string PrintNonStreaming(LLMClient& client, const std::string& prompt);

private:
    TUI& ui_;
    const Config& config_;
    DialogueContext context_;
};
