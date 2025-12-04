#pragma once

#include <functional>
#include <string>

#ifdef SendMessage
#undef SendMessage
#endif

class Config;

/**
 * Handles HTTP calls to the local Ollama API.
 */
class LLMClient {
public:
    // Builds a client configured from Config values.
    explicit LLMClient(const Config& config);

    // Releases libcurl global resources.
    ~LLMClient();

    // Sends a blocking request and returns the complete response text.
    std::string SendMessage(const std::string& prompt);

    // Sends a streaming request and emits chunks via callback.
    std::string SendMessageStream(const std::string& prompt,
                                  std::function<void(const std::string&)> onChunk);

private:
    std::string PerformRequest(const std::string& prompt,
                               bool stream,
                               std::function<void(const std::string&)> onChunk);

    std::string endpoint_;
    std::string model_;
    long timeoutSeconds_;
};
