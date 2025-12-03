#include "../include/image/ImageProcessor.h"
#include "../include/config/Config.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>

// Link OpenCV libraries
#ifdef _DEBUG
#pragma comment(lib, "opencv_core344d.lib")
#pragma comment(lib, "opencv_imgproc344d.lib")
#pragma comment(lib, "opencv_imgcodecs344d.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "ippicvmt.lib")
#pragma comment(lib, "ippiwd.lib")
#pragma comment(lib, "libprotobufd.lib")
#pragma comment(lib, "ittnotifyd.lib")
#pragma comment(lib, "libjpeg-turbod.lib")
#pragma comment(lib, "libpngd.lib")
#pragma comment(lib, "libtiffd.lib")
#pragma comment(lib, "libjasperd.lib")
#pragma comment(lib, "IlmImfd.lib")
#pragma comment(lib, "libwebpd.lib")
#else
#pragma comment(lib, "opencv_core344.lib")
#pragma comment(lib, "opencv_imgproc344.lib")
#pragma comment(lib, "opencv_imgcodecs344.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "ippicvmt.lib")
#pragma comment(lib, "ippiw.lib")
#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "ittnotify.lib")
#pragma comment(lib, "libjpeg-turbo.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "libtiff.lib")
#pragma comment(lib, "libjasper.lib")
#pragma comment(lib, "IlmImf.lib")
#pragma comment(lib, "libwebp.lib")
#endif

namespace image {

    ImageProcessor::ImageProcessor() = default;
    ImageProcessor::~ImageProcessor() = default;

    std::vector<float> ImageProcessor::preprocessImage(const std::filesystem::path& imagePath) {
        cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);
        if (image.empty()) {
            throw std::runtime_error("Failed to load image: " + imagePath.string());
        }
        return preprocessImage(image);
    }

    std::vector<float> ImageProcessor::preprocessImage(const cv::Mat& image) {
        cv::Mat processed;

        // Convert BGR to RGB
        cv::cvtColor(image, processed, cv::COLOR_BGR2RGB);

        // Resize to 224x224
        cv::Mat resized;
        cv::resize(processed, resized, cv::Size(config::IMAGE_SIZE, config::IMAGE_SIZE), 0, 0, cv::INTER_LINEAR);

        // Convert to tensor
        return matToTensor(resized);
    }

    std::vector<float> ImageProcessor::matToTensor(const cv::Mat& image) const {
        // Tensor shape: [1, 3, 224, 224]
        const int tensorSize = 1 * config::IMAGE_CHANNELS * config::IMAGE_SIZE * config::IMAGE_SIZE;
        std::vector<float> tensor(tensorSize);

        // OpenCV Mat is in HWC format (Height, Width, Channels)
        // We need CHW format (Channels, Height, Width)
        const int h = image.rows;
        const int w = image.cols;
        const int c = image.channels();

        // Mean and std for each channel
        const float means[] = { config::IMAGE_MEAN_R, config::IMAGE_MEAN_G, config::IMAGE_MEAN_B };
        const float stds[] = { config::IMAGE_STD_R, config::IMAGE_STD_G, config::IMAGE_STD_B };

        // Convert and normalize
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x);
                for (int ch = 0; ch < c; ++ch) {
                    // Normalize: (pixel / 255.0 - mean) / std
                    float normalized = normalizePixel(static_cast<float>(pixel[ch]), means[ch], stds[ch]);
                    // CHW format: [channel][height][width]
                    int idx = ch * (h * w) + y * w + x;
                    tensor[idx] = normalized;
                }
            }
        }

        return tensor;
    }

    float ImageProcessor::normalizePixel(float pixel, float mean, float std) const {
        return (pixel / 255.0f - mean) / std;
    }

} // namespace image

