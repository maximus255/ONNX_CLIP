#include "../include/text/BPETokenizer.h"
#include "../include/config/Config.h"
#include "../include/json.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace text {

    BPETokenizer::BPETokenizer() : loaded_(false) {}
    BPETokenizer::~BPETokenizer() = default;

    void BPETokenizer::load(const std::filesystem::path& encoderJsonPath, const std::filesystem::path& bpeVocabPath) {
        loadEncoder(encoderJsonPath);
        loadBPEVocab(bpeVocabPath);
        loaded_ = true;
    }

    void BPETokenizer::loadEncoder(const std::filesystem::path& encoderJsonPath) {
        std::ifstream file(encoderJsonPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open encoder JSON: " + encoderJsonPath.string());
        }

        nlohmann::json j;
        file >> j;

        encoder_.clear();
        for (auto& [key, value] : j.items()) {
            encoder_[key] = value.get<int32_t>();
        }
    }

    void BPETokenizer::loadBPEVocab(const std::filesystem::path& bpeVocabPath) {
        std::ifstream file(bpeVocabPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open BPE vocab: " + bpeVocabPath.string());
        }

        bpeMerges_.clear();
        std::string line;

        // Skip first line (version info)
        std::getline(file, line);

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string first, second;
            if (iss >> first >> second) {
                bpeMerges_.emplace_back(first, second);
            }
        }
    }

    std::vector<int32_t> BPETokenizer::tokenize(const std::string& text) {
        if (!loaded_) {
            throw std::runtime_error("Tokenizer not loaded");
        }

        // Split text into words (simple whitespace split)
        std::vector<std::string> words;
        std::istringstream iss(text);
        std::string word;
        while (iss >> word) {
            words.push_back(word);
        }

        // Convert words to character sequences
        std::vector<std::vector<std::string>> wordTokens;
        for (const auto& w : words) {
            std::vector<std::string> chars;
            for (char c : w) {
                chars.push_back(std::string(1, c));
            }
            // Add end-of-word marker
            if (!chars.empty()) {
                chars.back() += "</w>";
            }
            wordTokens.push_back(chars);
        }

        // Apply BPE to each word
        std::vector<std::string> allTokens;
        for (auto& wordToken : wordTokens) {
            auto bpeTokens = applyBPE(wordToken);
            allTokens.insert(allTokens.end(), bpeTokens.begin(), bpeTokens.end());
        }

        // Convert tokens to IDs
        std::vector<int32_t> tokenIds;
        for (const auto& token : allTokens) {
            auto it = encoder_.find(token);
            if (it != encoder_.end()) {
                tokenIds.push_back(it->second);
            } else {
                // Unknown token - use byte-level encoding
                for (unsigned char c : token) {
                    std::string byteToken = "<0x" + std::to_string(c) + ">";
                    auto byteIt = encoder_.find(byteToken);
                    if (byteIt != encoder_.end()) {
                        tokenIds.push_back(byteIt->second);
                    }
                }
            }
        }

        return tokenIds;
    }

    std::vector<std::string> BPETokenizer::applyBPE(const std::vector<std::string>& word) {
        std::vector<std::string> tokens = word;

        while (true) {
            auto merge = findBestMerge(tokens);
            if (merge.first == merge.second) {
                break; // No more merges possible
            }

            // Apply merge
            std::vector<std::string> newTokens;
            for (size_t i = 0; i < tokens.size(); ++i) {
                if (i == merge.first && i + 1 < tokens.size() && i + 1 == merge.second) {
                    newTokens.push_back(tokens[i] + tokens[i + 1]);
                    ++i; // Skip next token
                } else {
                    newTokens.push_back(tokens[i]);
                }
            }
            tokens = std::move(newTokens);
        }

        return tokens;
    }

    std::pair<size_t, size_t> BPETokenizer::findBestMerge(const std::vector<std::string>& word) {
        if (word.size() < 2) {
            return {0, 0}; // No merge possible
        }

        // Find first valid merge from BPE merges list
        for (size_t i = 0; i < word.size() - 1; ++i) {
            const std::string& first = word[i];
            const std::string& second = word[i + 1];

            // Check if this pair exists in BPE merges
            for (size_t j = 0; j < bpeMerges_.size(); ++j) {
                if (bpeMerges_[j].first == first && bpeMerges_[j].second == second) {
                    return {i, i + 1};
                }
            }
        }

        return {0, 0}; // No merge found
    }

} // namespace text

