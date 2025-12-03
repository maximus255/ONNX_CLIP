#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include "BPETokenizer.h"

namespace text {

    class Tokenizer {
    public:
        Tokenizer();
        ~Tokenizer();

        // Initialize tokenizer with paths
        void initialize(
            const std::filesystem::path& encoderJsonPath,
            const std::filesystem::path& bpeVocabPath
        );

        // Tokenize text and pad to context length
        // Returns [context_length] vector
        std::vector<int32_t> tokenize(const std::string& text);

        // Tokenize multiple texts
        std::vector<std::vector<int32_t>> tokenizeBatch(const std::vector<std::string>& texts);

    private:
        std::unique_ptr<BPETokenizer> bpeTokenizer_;
        bool initialized_;
    };

} // namespace text

