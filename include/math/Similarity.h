#pragma once

#include <vector>

namespace math {

    // Compute cosine similarity between two vectors
    float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);

    // Compute cosine similarity matrix
    // a: [N, D], b: [M, D] -> [N, M]
    std::vector<std::vector<float>> cosineSimilarityMatrix(
        const std::vector<std::vector<float>>& a,
        const std::vector<std::vector<float>>& b
    );

    // Compute L2 norm of vector
    float l2Norm(const std::vector<float>& vec);

} // namespace math

