#include "../include/utils/FileUtils.h"
#include "../include/config/Config.h"
#include <fstream>
#include <algorithm>
#include <cctype>

namespace utils {

    std::vector<std::string> loadTextClasses(const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error("File not found: " + filePath.string());
        }

        std::vector<std::string> texts;
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filePath.string());
        }

        std::string line;
        while (std::getline(file, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            texts.push_back(line);
        }

        if (texts.empty()) {
            throw std::runtime_error("No valid text lines found in: " + filePath.string());
        }

        return texts;
    }

    std::vector<std::filesystem::path> loadTestImages(
        const std::filesystem::path& dirPath,
        int maxImages
    ) {
        if (!std::filesystem::exists(dirPath)) {
            throw std::runtime_error("Directory not found: " + dirPath.string());
        }

        if (!std::filesystem::is_directory(dirPath)) {
            throw std::runtime_error("Path is not a directory: " + dirPath.string());
        }

        std::vector<std::filesystem::path> paths;

        // Recursively search for image files
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file() && isImageFile(entry.path())) {
                paths.push_back(entry.path());
            }
        }

        // Sort paths
        std::sort(paths.begin(), paths.end());

        // Limit number of images
        if (paths.size() > static_cast<size_t>(maxImages)) {
            paths.resize(maxImages);
        }

        if (paths.empty()) {
            throw std::runtime_error("No images found in: " + dirPath.string());
        }

        return paths;
    }

    bool isImageFile(const std::filesystem::path& filePath) {
        std::string ext = filePath.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        return ext == config::IMAGE_EXT_JPG ||
               ext == config::IMAGE_EXT_JPEG ||
               ext == config::IMAGE_EXT_PNG;
    }

} // namespace utils

