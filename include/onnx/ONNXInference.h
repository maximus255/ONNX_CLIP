#pragma once

#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>

namespace onnx {

    class ONNXSession {
    public:
        ONNXSession(Ort::Env& env, const std::filesystem::path& modelPath);
        ~ONNXSession();

        // Move-only semantics
        ONNXSession(ONNXSession&&) = default;
        ONNXSession& operator=(ONNXSession&&) = default;
        ONNXSession(const ONNXSession&) = delete;
        ONNXSession& operator=(const ONNXSession&) = delete;

        // Run inference
        std::vector<float> run(const std::string& inputName, const std::vector<float>& input);
        std::vector<float> run(const std::string& inputName, const std::vector<int32_t>& input);

        // Get input/output names
        std::string getInputName(size_t index = 0) const;
        std::string getOutputName(size_t index = 0) const;

    private:
        Ort::Session session_;
        std::vector<const char*> inputNames_;
        std::vector<const char*> outputNames_;
    };

} // namespace onnx

