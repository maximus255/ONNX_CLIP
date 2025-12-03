#include "../include/onnx/ONNXInference.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdexcept>
#include <cstring>

namespace onnx {

    ONNXSession::ONNXSession(Ort::Env& env, const std::filesystem::path& modelPath)
        : session_(env, modelPath.wstring().c_str(), Ort::SessionOptions{nullptr}) {

        // Get input/output names using Ort::AllocatorWithDefaultOptions
        size_t numInputNodes = session_.GetInputCount();
        size_t numOutputNodes = session_.GetOutputCount();

        inputNames_.reserve(numInputNodes);
        outputNames_.reserve(numOutputNodes);

        Ort::AllocatorWithDefaultOptions allocator;
        for (size_t i = 0; i < numInputNodes; ++i) {
            auto inputName = session_.GetInputNameAllocated(i, allocator);
            std::string nameStr(inputName.get());
            char* nameCopy = new char[nameStr.length() + 1];
            strcpy_s(nameCopy, nameStr.length() + 1, nameStr.c_str());
            inputNames_.push_back(nameCopy);
        }

        for (size_t i = 0; i < numOutputNodes; ++i) {
            auto outputName = session_.GetOutputNameAllocated(i, allocator);
            std::string nameStr(outputName.get());
            char* nameCopy = new char[nameStr.length() + 1];
            strcpy_s(nameCopy, nameStr.length() + 1, nameStr.c_str());
            outputNames_.push_back(nameCopy);
        }
    }

    ONNXSession::~ONNXSession() {
        // Free allocated names
        for (const char* name : inputNames_) {
            delete[] name;
        }
        for (const char* name : outputNames_) {
            delete[] name;
        }
    }

    std::vector<float> ONNXSession::run(const std::string& inputName, const std::vector<float>& input) {
        // Create input tensor
        std::vector<int64_t> inputShape = {1, 3, 224, 224}; // [batch, channels, height, width]
        size_t inputTensorSize = input.size();
        
        auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
            memoryInfo, const_cast<float*>(input.data()), inputTensorSize,
            inputShape.data(), inputShape.size()
        );

        // Run inference
        const char* inputNames[] = {inputName.c_str()};
        const char* outputNames[] = {outputNames_[0]};

        auto outputTensors = session_.Run(Ort::RunOptions{nullptr},
            inputNames, &inputTensor, 1,
            outputNames, 1);

        // Extract output
        float* floatArray = outputTensors.front().GetTensorMutableData<float>();
        auto outputShape = outputTensors.front().GetTensorTypeAndShapeInfo().GetShape();
        size_t outputSize = 1;
        for (auto dim : outputShape) {
            outputSize *= dim;
        }

        std::vector<float> result(floatArray, floatArray + outputSize);
        return result;
    }

    std::vector<float> ONNXSession::run(const std::string& inputName, const std::vector<int32_t>& input) {
        // Create input tensor
        std::vector<int64_t> inputShape = {1, static_cast<int64_t>(input.size())}; // [batch, sequence_length]
        size_t inputTensorSize = input.size();
        
        auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value inputTensor = Ort::Value::CreateTensor<int32_t>(
            memoryInfo, const_cast<int32_t*>(input.data()), inputTensorSize,
            inputShape.data(), inputShape.size()
        );

        // Run inference
        const char* inputNames[] = {inputName.c_str()};
        const char* outputNames[] = {outputNames_[0]};

        auto outputTensors = session_.Run(Ort::RunOptions{nullptr},
            inputNames, &inputTensor, 1,
            outputNames, 1);

        // Extract output
        float* floatArray = outputTensors.front().GetTensorMutableData<float>();
        auto outputShape = outputTensors.front().GetTensorTypeAndShapeInfo().GetShape();
        size_t outputSize = 1;
        for (auto dim : outputShape) {
            outputSize *= dim;
        }

        std::vector<float> result(floatArray, floatArray + outputSize);
        return result;
    }

    std::string ONNXSession::getInputName(size_t index) const {
        if (index >= inputNames_.size()) {
            throw std::runtime_error("Input index out of range");
        }
        return std::string(inputNames_[index]);
    }

    std::string ONNXSession::getOutputName(size_t index) const {
        if (index >= outputNames_.size()) {
            throw std::runtime_error("Output index out of range");
        }
        return std::string(outputNames_[index]);
    }

} // namespace onnx

