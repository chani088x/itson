#include "LLMClient.h"

#ifdef SendMessage
#undef SendMessage
#endif

#include <algorithm>
#include <sstream>
#include <utility>

#include <curl/curl.h>

#ifdef SendMessage
#undef SendMessage
#endif

#include <nlohmann/json.hpp>

#include "Config.h"

namespace {
constexpr std::size_t kMaxStreamBufferBytes = 1 * 1024 * 1024;  // cap streaming buffers

size_t WriteToString(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* buffer = static_cast<std::string*>(userdata);
    buffer->append(ptr, size * nmemb);
    return size * nmemb;
}

struct StreamState {
    std::string pending;
    std::string fullResponse;
    std::function<void(const std::string&)> onChunk;
    bool done = false;
};

size_t WriteStream(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* state = static_cast<StreamState*>(userdata);
    const std::size_t incoming = size * nmemb;

    if (state->pending.size() + incoming > kMaxStreamBufferBytes) {
        // Prevent unbounded growth; ignore excess data but consume to allow transfer completion.
        return incoming;
    }

    state->pending.append(ptr, incoming);

    std::size_t newlinePos = state->pending.find('\n');
    while (newlinePos != std::string::npos) {
        std::string line = state->pending.substr(0, newlinePos);
        state->pending.erase(0, newlinePos + 1);
        if (!line.empty()) {
            try {
                auto jsonLine = nlohmann::json::parse(line);
                if (jsonLine.value("response", "").size()) {
                    std::string chunk = jsonLine["response"].get<std::string>();
                    if (state->fullResponse.size() + chunk.size() <= kMaxStreamBufferBytes) {
                        state->fullResponse += chunk;
                    }
                    if (state->onChunk) {
                        state->onChunk(chunk);
                    }
                }
                if (jsonLine.value("done", false)) {
                    state->done = true;
                }
            } catch (const std::exception&) {
                // Ignore malformed chunks but keep streaming.
            }
        }
        newlinePos = state->pending.find('\n');
    }
    return size * nmemb;
}

void FlushPending(StreamState& state) {
    if (state.pending.empty()) {
        return;
    }
    try {
        auto jsonLine = nlohmann::json::parse(state.pending);
        if (jsonLine.value("response", "").size()) {
            std::string chunk = jsonLine["response"].get<std::string>();
            if (state.fullResponse.size() + chunk.size() <= kMaxStreamBufferBytes) {
                state.fullResponse += chunk;
            }
            if (state.onChunk) {
                state.onChunk(chunk);
            }
        }
        if (jsonLine.value("done", false)) {
            state.done = true;
        }
    } catch (const std::exception&) {
    }
    state.pending.clear();
}

std::string ParseBlockingResponse(const std::string& body) {
    try {
        auto json = nlohmann::json::parse(body);
        if (json.contains("response")) {
            return json["response"].get<std::string>();
        }
        if (json.contains("error")) {
            return "Error: " + json["error"].get<std::string>();
        }
        std::ostringstream oss;
        oss << "Unexpected response: " << body;
        return oss.str();
    } catch (const std::exception& e) {
        return std::string("Error parsing LLM response: ") + e.what();
    }
}
}  // namespace

LLMClient::LLMClient(const Config& config)
    : endpoint_(config.GetEndpoint()),
      model_(config.GetModel()),
      timeoutSeconds_(60) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

LLMClient::~LLMClient() {
    curl_global_cleanup();
}

std::string LLMClient::SendMessage(const std::string& prompt) {
    return PerformRequest(prompt, false, nullptr);
}

std::string LLMClient::SendMessageStream(const std::string& prompt,
                                         std::function<void(const std::string&)> onChunk) {
    return PerformRequest(prompt, true, std::move(onChunk));
}

std::string LLMClient::PerformRequest(const std::string& prompt,
                                      bool stream,
                                      std::function<void(const std::string&)> onChunk) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "Error: curl_easy_init failed";
    }

    nlohmann::json payload = {
        {"model", model_},
        {"prompt", prompt},
        {"stream", stream},
    };
    std::string payloadString = payload.dump();

    // Basic scheme allowlist to avoid unintended protocols.
    auto is_http_scheme = [](const std::string& url) {
        return url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0;
    };
    if (!is_http_scheme(endpoint_)) {
        return "Error: unsupported endpoint scheme";
    }

    curl_easy_setopt(curl, CURLOPT_URL, endpoint_.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadString.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payloadString.size());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds_);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Verify TLS when using HTTPS.
    if (endpoint_.rfind("https://", 0) == 0) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    }

    std::string blockingBuffer;
    StreamState streamState;
    streamState.onChunk = std::move(onChunk);

    if (stream) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStream);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &streamState);
    } else {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &blockingBuffer);
    }

    CURLcode result = curl_easy_perform(curl);
    if (stream) {
        FlushPending(streamState);
    }

    long statusCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        return std::string("Error: ") + curl_easy_strerror(result);
    }
    if (statusCode >= 400) {
        std::ostringstream oss;
        oss << "HTTP Error " << statusCode;
        return oss.str();
    }

    if (stream) {
        return streamState.fullResponse;
    }
    return ParseBlockingResponse(blockingBuffer);
}
