#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace text {

    class BPETokenizer {
    public:
        BPETokenizer();
        ~BPETokenizer();

        // Load tokenizer files
        void load(const std::filesystem::path& encoderJsonPath, const std::filesystem::path& bpeVocabPath);

        // Tokenize text into token IDs
        std::vector<int32_t> tokenize(const std::string& text);

    private:
        // Load encoder JSON
        void loadEncoder(const std::filesystem::path& encoderJsonPath);

        // Load BPE vocabulary
        void loadBPEVocab(const std::filesystem::path& bpeVocabPath);

        // Apply BPE merges to word
        std::vector<std::string> applyBPE(const std::vector<std::string>& word);

        // Find best BPE merge
        std::pair<size_t, size_t> findBestMerge(const std::vector<std::string>& word);

        std::unordered_map<std::string, int32_t> encoder_;  // token -> id
        std::vector<std::pair<std::string, std::string>> bpeMerges_;  // BPE merge pairs
        bool loaded_;
    };

} // namespace text

