#pragma once

#include <vector>
#include <filesystem>
#include <memory>

// Forward declaration
namespace cv {
    class Mat;
}

namespace image {

    class ImageProcessor {
    public:
        ImageProcessor();
        ~ImageProcessor();

        // Load and preprocess image from file
        // Returns tensor as [1, 3, 224, 224] float32 vector
        std::vector<float> preprocessImage(const std::filesystem::path& imagePath);

        // Preprocess already loaded image
        std::vector<float> preprocessImage(const cv::Mat& image);

    private:
        // Convert cv::Mat to tensor format [1, 3, 224, 224]
        std::vector<float> matToTensor(const cv::Mat& image) const;

        // Normalize pixel values
        float normalizePixel(float pixel, float mean, float std) const;
    };

} // namespace image

