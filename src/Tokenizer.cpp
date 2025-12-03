#include "../include/text/Tokenizer.h"
#include "../include/config/Config.h"
#include <algorithm>
#include <stdexcept>

namespace text {

    Tokenizer::Tokenizer() : initialized_(false) {
        bpeTokenizer_ = std::make_unique<BPETokenizer>();
    }

    Tokenizer::~Tokenizer() = default;

    void Tokenizer::initialize(
        const std::filesystem::path& encoderJsonPath,
        const std::filesystem::path& bpeVocabPath
    ) {
        bpeTokenizer_->load(encoderJsonPath, bpeVocabPath);
        initialized_ = true;
    }

    std::vector<int32_t> Tokenizer::tokenize(const std::string& text) {
        if (!initialized_) {
            throw std::runtime_error("Tokenizer not initialized");
        }

        // Get BPE tokens
        std::vector<int32_t> tokens = bpeTokenizer_->tokenize(text);

        // Add special tokens: [SOT] + tokens + [EOT]
        std::vector<int32_t> result;
        result.reserve(config::CONTEXT_LENGTH);
        result.push_back(config::TOKEN_START_OF_TEXT);
        result.insert(result.end(), tokens.begin(), tokens.end());
        result.push_back(config::TOKEN_END_OF_TEXT);

        // Truncate if too long
        if (result.size() > static_cast<size_t>(config::CONTEXT_LENGTH)) {
            result.resize(config::CONTEXT_LENGTH);
            result[config::CONTEXT_LENGTH - 1] = config::TOKEN_END_OF_TEXT;
        }

        // Pad to context length
        while (result.size() < static_cast<size_t>(config::CONTEXT_LENGTH)) {
            result.push_back(config::TOKEN_PAD);
        }

        return result;
    }

    std::vector<std::vector<int32_t>> Tokenizer::tokenizeBatch(const std::vector<std::string>& texts) {
        std::vector<std::vector<int32_t>> result;
        result.reserve(texts.size());

        for (const auto& text : texts) {
            result.push_back(tokenize(text));
        }

        return result;
    }

} // namespace text

