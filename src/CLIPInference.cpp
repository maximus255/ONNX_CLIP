#include "../include/clip/CLIPInference.h"
#include "../include/config/Config.h"
#include <stdexcept>

namespace clip {

    CLIPInference::CLIPInference(const std::filesystem::path& modelsDir)
        : env_(ORT_LOGGING_LEVEL_WARNING, "CLIPInference") {

        // Load models
        auto imageModelPath = modelsDir / config::IMAGE_ENCODER_MODEL;
        auto textModelPath = modelsDir / config::TEXT_ENCODER_MODEL;

        imageSession_ = std::make_unique<onnx::ONNXSession>(env_, imageModelPath);
        textSession_ = std::make_unique<onnx::ONNXSession>(env_, textModelPath);

        // Initialize tokenizer
        auto encoderJsonPath = modelsDir / config::TOKENIZER_ENCODER_JSON;
        auto bpeVocabPath = modelsDir / config::BPE_VOCAB_FILE;

        tokenizer_ = std::make_unique<text::Tokenizer>();
        tokenizer_->initialize(encoderJsonPath, bpeVocabPath);

        // Initialize image processor
        imageProcessor_ = std::make_unique<image::ImageProcessor>();
    }

    CLIPInference::~CLIPInference() = default;

    std::vector<float> CLIPInference::encodeImage(const std::filesystem::path& imagePath) {
        // Preprocess image
        std::vector<float> imageTensor = imageProcessor_->preprocessImage(imagePath);

        // Run image encoder
        std::string inputName = imageSession_->getInputName(0);
        return imageSession_->run(inputName, imageTensor);
    }

    std::vector<float> CLIPInference::encodeText(const std::string& text) {
        // Tokenize text
        std::vector<int32_t> tokens = tokenizer_->tokenize(text);

        // Run text encoder
        std::string inputName = textSession_->getInputName(0);
        return textSession_->run(inputName, tokens);
    }

    std::vector<std::vector<float>> CLIPInference::encodeTexts(const std::vector<std::string>& texts) {
        // Check if we need to recompute
        bool needsRecompute = false;
        if (cachedTexts_.size() != texts.size()) {
            needsRecompute = true;
        } else {
            for (size_t i = 0; i < texts.size(); ++i) {
                if (cachedTexts_[i] != texts[i]) {
                    needsRecompute = true;
                    break;
                }
            }
        }

        if (!needsRecompute) {
            return cachedTextEmbeddings_;
        }

        // Compute embeddings
        cachedTexts_ = texts;
        cachedTextEmbeddings_.clear();
        cachedTextEmbeddings_.reserve(texts.size());

        for (const auto& text : texts) {
            cachedTextEmbeddings_.push_back(encodeText(text));
        }

        return cachedTextEmbeddings_;
    }

} // namespace clip

