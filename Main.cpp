#include <iostream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include "include/cxxopts.hpp"
#include "include/config/Config.h"
#include "include/utils/FileUtils.h"
#include "include/clip/CLIPInference.h"
#include "include/math/Similarity.h"

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        cxxopts::Options options("ONNX_CPP", "CLIP inference using ONNX Runtime");

        options.add_options()
            ("i,images", "Path to images directory", cxxopts::value<std::string>())
            ("k,topk", "Number of top matches to show", cxxopts::value<int>()->default_value(std::to_string(config::DEFAULT_TOP_K)))
            ("m,max-images", "Maximum number of images to process", cxxopts::value<int>()->default_value(std::to_string(config::DEFAULT_MAX_IMAGES)))
            ("h,help", "Print help");

        auto result = options.parse(argc, argv);

        if (result.count("help") || !result.count("images")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        std::filesystem::path imagesDir = result["images"].as<std::string>();
        int topK = result["topk"].as<int>();
        int maxImages = result["max-images"].as<int>();

        // Load text classes
        // Try to find classes.txt relative to executable or use absolute path
        std::filesystem::path classesTxt;
        std::filesystem::path baseDir = std::filesystem::current_path().parent_path();
        std::filesystem::path testPath = baseDir / "classes.txt";
        if (std::filesystem::exists(testPath)) {
            classesTxt = testPath;
        } else {
            // Fallback to absolute path
            classesTxt = std::filesystem::path(config::MODELS_DIR).parent_path() / "classes.txt";
        }
        std::vector<std::string> texts = utils::loadTextClasses(classesTxt);
        std::cout << "Loaded " << texts.size() << " text classes from " << classesTxt.filename().string() << "." << std::endl;

        // Load images
        std::vector<std::filesystem::path> imagePaths = utils::loadTestImages(imagesDir, maxImages);
        std::cout << "Found " << imagePaths.size() << " images (limit: " << maxImages << "):" << std::endl;
        for (const auto& path : imagePaths) {
            std::cout << "  " << std::filesystem::relative(path, imagesDir).string() << std::endl;
        }

        // Initialize CLIP
        std::cout << "\n=== Loading ONNX models ===" << std::endl;
        std::filesystem::path modelsDir = config::MODELS_DIR;
        clip::CLIPInference clip(modelsDir);
        std::cout << "Models loaded successfully!" << std::endl;

        // Encode texts (with caching)
        std::cout << "\n=== Encoding text classes ===" << std::endl;
        std::vector<std::vector<float>> textEmbeddings = clip.encodeTexts(texts);
        std::cout << "Encoded " << textEmbeddings.size() << " text classes." << std::endl;

        // Encode images
        std::cout << "\n=== Encoding images ===" << std::endl;
        std::vector<std::vector<float>> imageEmbeddings;
        imageEmbeddings.reserve(imagePaths.size());

        for (size_t i = 0; i < imagePaths.size(); ++i) {
            try {
                std::vector<float> embedding = clip.encodeImage(imagePaths[i]);
                imageEmbeddings.push_back(embedding);
                std::cout << "Encoded image " << (i + 1) << "/" << imagePaths.size() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error encoding image " << imagePaths[i] << ": " << e.what() << std::endl;
                // Add zero embedding as placeholder
                imageEmbeddings.push_back(std::vector<float>(config::EMBEDDING_DIM, 0.0f));
            }
        }

        // Compute cosine similarity
        std::cout << "\n=== Computing cosine similarity ===" << std::endl;
        std::vector<std::vector<float>> similarityMatrix = math::cosineSimilarityMatrix(imageEmbeddings, textEmbeddings);

        // Print top-K results
        std::cout << "\n=== Top-" << topK << " text descriptions for each image ===" << std::endl;

        for (size_t i = 0; i < imagePaths.size(); ++i) {
            const auto& scores = similarityMatrix[i];
            
            // Create indices and sort by score
            std::vector<size_t> indices(scores.size());
            std::iota(indices.begin(), indices.end(), 0);
            std::sort(indices.begin(), indices.end(), [&scores](size_t a, size_t b) {
                return scores[a] > scores[b];
            });

            std::cout << "\nImage: " << std::filesystem::relative(imagePaths[i], imagesDir).string() << std::endl;
            for (int rank = 0; rank < topK && rank < static_cast<int>(indices.size()); ++rank) {
                size_t idx = indices[rank];
                std::cout << "  " << (rank + 1) << ". score=" << std::fixed << std::setprecision(4) 
                          << scores[idx] << " | " << texts[idx] << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

