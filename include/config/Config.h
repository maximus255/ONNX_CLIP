#pragma once

#include <string>

namespace config {

    // Model paths (relative to models directory provided via CLI)
    inline constexpr const char* IMAGE_ENCODER_MODEL = "image_encoder.onnx";
    inline constexpr const char* TEXT_ENCODER_MODEL = "text_encoder.onnx";
    inline constexpr const char* TOKENIZER_ENCODER_JSON = "tokenizer_encoder.json";
    inline constexpr const char* BPE_VOCAB_FILE = "bpe_simple_vocab_16e6.txt";

    // Model specifications
    inline constexpr int CONTEXT_LENGTH = 77;
    inline constexpr int EMBEDDING_DIM = 512;
    inline constexpr int IMAGE_SIZE = 224;
    inline constexpr int IMAGE_CHANNELS = 3;

    // Special tokens
    inline constexpr int TOKEN_START_OF_TEXT = 49406;
    inline constexpr int TOKEN_END_OF_TEXT = 49407;
    inline constexpr int TOKEN_PAD = 0;

    // Image preprocessing
    inline constexpr float IMAGE_MEAN_R = 0.485f;
    inline constexpr float IMAGE_MEAN_G = 0.456f;
    inline constexpr float IMAGE_MEAN_B = 0.406f;
    inline constexpr float IMAGE_STD_R = 0.229f;
    inline constexpr float IMAGE_STD_G = 0.224f;
    inline constexpr float IMAGE_STD_B = 0.225f;

    // Default values
    inline constexpr int DEFAULT_MAX_IMAGES = 10;
    inline constexpr int DEFAULT_TOP_K = 3;

    // Image file extensions
    inline constexpr const char* IMAGE_EXT_JPG = ".jpg";
    inline constexpr const char* IMAGE_EXT_JPEG = ".jpeg";
    inline constexpr const char* IMAGE_EXT_PNG = ".png";

} // namespace config

