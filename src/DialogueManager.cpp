#include "DialogueManager.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <utility>
#include <vector>

#include "Config.h"
#include "TUI.h"
#include "LLMClient.h"

namespace {
std::string ToLower(const std::string& text) {
    std::string lowered = text;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return lowered;
}
}  // namespace

void DialogueContext::AddTurn(const std::string& speaker, const std::string& text) {
    history_.push_back({speaker, text});
}

std::string DialogueContext::ToHistoryString(std::size_t limit) const {
    if (history_.empty()) {
        return "대화 기록 없음.";
    }
    std::ostringstream oss;
    std::size_t start = history_.size() > limit ? history_.size() - limit : 0;
    for (std::size_t i = start; i < history_.size(); ++i) {
        oss << history_[i].speaker << ": " << history_[i].text << '\n';
    }
    return oss.str();
}

void DialogueContext::Clear() {
    history_.clear();
}

const std::vector<DialogueTurn>& DialogueContext::History() const {
    return history_;
}

DialogueManager::DialogueManager(TUI& ui, const Config& config)
    : ui_(ui), config_(config) {}

DialogueContext& DialogueManager::GetContext() {
    return context_;
}

const DialogueContext& DialogueManager::GetContext() const {
    return context_;
}

int DialogueManager::ScoreAffectionDelta(const std::string& userText, const std::string& npcText) const {
    const std::vector<std::pair<std::string, int>> rules = {
        {"고마워", 2}, {"좋아해", 4}, {"사랑", 5}, {"미안", 1},
        {"칭찬", 2},  {"최고", 3},  {"싫어", -4}, {"짜증", -3},
        {"별로", -2}, {"씨발", -5},
    };

    std::string joined = ToLower(userText);
    int delta = 0;
    for (const auto& [keyword, score] : rules) {
        if (joined.find(keyword) != std::string::npos) {
            delta += score;
        }
    }
    delta = std::max(-5, std::min(5, delta));
    return delta;
}

std::string DialogueManager::PrintStreaming(LLMClient& client, const std::string& prompt) {
    // ui_.SetColorNPC(); // TUI doesn't have color methods yet or handles them differently
    std::string reply = client.SendMessageStream(prompt, [this](const std::string& chunk) {
        ui_.PrintChunk(chunk);
    });
    ui_.NewLine();
    // ui_.ResetColor();
    return reply;
}

std::string DialogueManager::PrintNonStreaming(LLMClient& client, const std::string& prompt) {
    std::string reply = client.SendMessage(prompt);
    // ui_.SetColorNPC();
    ui_.PrintChunk(reply); // TUI doesn't have TypeOut, use PrintChunk or similar
    ui_.NewLine();
    // ui_.ResetColor();
    return reply;
}
