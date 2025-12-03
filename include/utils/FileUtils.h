#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace utils {

    // Load text classes from file, ignoring empty lines and comments starting with '#'
    std::vector<std::string> loadTextClasses(const std::filesystem::path& filePath);

    // Recursively find image files in directory
    std::vector<std::filesystem::path> loadTestImages(
        const std::filesystem::path& dirPath,
        int maxImages = 10
    );

    // Check if file has image extension
    bool isImageFile(const std::filesystem::path& filePath);

} // namespace utils

