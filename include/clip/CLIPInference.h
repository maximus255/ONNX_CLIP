#pragma once

#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>
#include "../onnx/ONNXInference.h"
#include "../text/Tokenizer.h"
#include "../image/ImageProcessor.h"

namespace clip {

    class CLIPInference {
    public:
        CLIPInference(const std::filesystem::path& modelsDir);
        ~CLIPInference();

        // Encode image to embedding
        std::vector<float> encodeImage(const std::filesystem::path& imagePath);

        // Encode text to embedding
        std::vector<float> encodeText(const std::string& text);

        // Encode multiple texts (with caching)
        std::vector<std::vector<float>> encodeTexts(const std::vector<std::string>& texts);

        // Get cached text embeddings
        const std::vector<std::vector<float>>& getCachedTextEmbeddings() const { return cachedTextEmbeddings_; }

    private:
        Ort::Env env_;
        std::unique_ptr<onnx::ONNXSession> imageSession_;
        std::unique_ptr<onnx::ONNXSession> textSession_;
        std::unique_ptr<text::Tokenizer> tokenizer_;
        std::unique_ptr<image::ImageProcessor> imageProcessor_;

        // Caching
        std::vector<std::string> cachedTexts_;
        std::vector<std::vector<float>> cachedTextEmbeddings_;
    };

} // namespace clip

