#include "../include/math/Similarity.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace math {

    float l2Norm(const std::vector<float>& vec) {
        float sum = 0.0f;
        for (float v : vec) {
            sum += v * v;
        }
        return std::sqrt(sum);
    }

    float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::runtime_error("Vector sizes must match for cosine similarity");
        }

        // Dot product
        float dotProduct = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dotProduct += a[i] * b[i];
        }

        // Norms
        float normA = l2Norm(a);
        float normB = l2Norm(b);

        if (normA == 0.0f || normB == 0.0f) {
            return 0.0f;
        }

        return dotProduct / (normA * normB);
    }

    std::vector<std::vector<float>> cosineSimilarityMatrix(
        const std::vector<std::vector<float>>& a,
        const std::vector<std::vector<float>>& b
    ) {
        if (a.empty() || b.empty()) {
            return {};
        }

        // Normalize all vectors
        std::vector<std::vector<float>> aNorm(a.size());
        std::vector<std::vector<float>> bNorm(b.size());

        for (size_t i = 0; i < a.size(); ++i) {
            float norm = l2Norm(a[i]);
            if (norm > 0.0f) {
                aNorm[i].resize(a[i].size());
                for (size_t j = 0; j < a[i].size(); ++j) {
                    aNorm[i][j] = a[i][j] / norm;
                }
            } else {
                aNorm[i] = a[i];
            }
        }

        for (size_t i = 0; i < b.size(); ++i) {
            float norm = l2Norm(b[i]);
            if (norm > 0.0f) {
                bNorm[i].resize(b[i].size());
                for (size_t j = 0; j < b[i].size(); ++j) {
                    bNorm[i][j] = b[i][j] / norm;
                }
            } else {
                bNorm[i] = b[i];
            }
        }

        // Compute similarity matrix
        std::vector<std::vector<float>> result(a.size(), std::vector<float>(b.size()));

        for (size_t i = 0; i < a.size(); ++i) {
            for (size_t j = 0; j < b.size(); ++j) {
                float dotProduct = 0.0f;
                for (size_t k = 0; k < aNorm[i].size(); ++k) {
                    dotProduct += aNorm[i][k] * bNorm[j][k];
                }
                result[i][j] = dotProduct;
            }
        }

        return result;
    }

} // namespace math

