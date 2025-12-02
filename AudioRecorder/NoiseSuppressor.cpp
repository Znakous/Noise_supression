#include "NoiseSuppressor.hpp"
#include "rnnoise.h"
#include <algorithm>
#include <cmath>
#include <cstring>

// Definition of custom deleter for DenoiseState
void DenoiseDeleter::operator()(DenoiseState* ptr) const noexcept {
    if (ptr) {
        rnnoise_destroy(ptr);
    }
}

NoiseSuppressor::NoiseSuppressor()
    : _denoiseState(rnnoise_create(nullptr))
    , _enabled(false)
    , _currentInputRate(44100)
    , _currentOutputRate(44100) {
    _inputBuffer.reserve(480 * 2); // Reserve space for at least 2 frames
}

NoiseSuppressor::~NoiseSuppressor() = default;

void NoiseSuppressor::SetEnabled(bool enabled) {
    _enabled = enabled;
    if (!enabled) {
        // Clear buffer when disabling
        _inputBuffer.clear();
    }
}

bool NoiseSuppressor::IsEnabled() const {
    return _enabled;
}

void NoiseSuppressor::Int16ToFloat32(const int16_t* input, float* output, size_t count) {
    const float scale = 1.0f / 32768.0f;
    for (size_t i = 0; i < count; ++i) {
        output[i] = static_cast<float>(input[i]) * scale;
    }
}

void NoiseSuppressor::Float32ToInt16(const float* input, int16_t* output, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        float sample = input[i];
        // Clamp to [-1.0, 1.0]
        sample = std::max(-1.0f, std::min(1.0f, sample));
        output[i] = static_cast<int16_t>(sample * 32767.0f);
    }
}

std::vector<float> NoiseSuppressor::Resample(const float* input, size_t inputSamples, 
                                              unsigned int inputRate, unsigned int outputRate) {
    if (inputRate == outputRate) {
        return std::vector<float>(input, input + inputSamples);
    }

    // Simple linear interpolation resampling
    const double ratio = static_cast<double>(outputRate) / static_cast<double>(inputRate);
    const size_t outputSamples = static_cast<size_t>(std::round(inputSamples * ratio));
    std::vector<float> output(outputSamples);

    for (size_t i = 0; i < outputSamples; ++i) {
        const double srcIndex = i / ratio;
        const size_t srcIndex0 = static_cast<size_t>(srcIndex);
        const size_t srcIndex1 = std::min(srcIndex0 + 1, inputSamples - 1);
        const double t = srcIndex - srcIndex0;

        if (srcIndex0 < inputSamples) {
            output[i] = input[srcIndex0] * (1.0 - t) + input[srcIndex1] * t;
        } else {
            output[i] = input[inputSamples - 1];
        }
    }

    return output;
}

void NoiseSuppressor::ProcessFrame(float* frame) {
    if (!_denoiseState || !_enabled) {
        return;
    }
    
    // rnnoise_process_frame processes in-place
    rnnoise_process_frame(_denoiseState.get(), frame, frame);
}

std::vector<int16_t> NoiseSuppressor::ProcessSamples(const int16_t* samples, size_t numSamples,
                                                     unsigned int inputSampleRate, 
                                                     unsigned int outputSampleRate) {
    if (!_enabled || numSamples == 0) {
        // If disabled, just return input samples (may need resampling)
        if (inputSampleRate != outputSampleRate) {
            std::vector<float> floatInput(numSamples);
            Int16ToFloat32(samples, floatInput.data(), numSamples);
            auto resampled = Resample(floatInput.data(), numSamples, inputSampleRate, outputSampleRate);
            std::vector<int16_t> result(resampled.size());
            Float32ToInt16(resampled.data(), result.data(), resampled.size());
            return result;
        }
        return std::vector<int16_t>(samples, samples + numSamples);
    }

    _currentInputRate = inputSampleRate;
    _currentOutputRate = outputSampleRate;

    // Convert int16_t to float32
    std::vector<float> floatInput(numSamples);
    Int16ToFloat32(samples, floatInput.data(), numSamples);

    // Resample to 48kHz if needed (rnnoise requires 48kHz)
    std::vector<float> resampled48k;
    if (inputSampleRate != 48000) {
        resampled48k = Resample(floatInput.data(), numSamples, inputSampleRate, 48000);
    } else {
        resampled48k = std::move(floatInput);
    }

    // Add to input buffer
    _inputBuffer.insert(_inputBuffer.end(), resampled48k.begin(), resampled48k.end());

    // Process complete frames (480 samples each at 48kHz)
    const size_t frameSize = 480;
    std::vector<float> processedFrames;
    processedFrames.reserve(_inputBuffer.size());

    while (_inputBuffer.size() >= frameSize) {
        // Extract one frame
        float frame[frameSize];
        std::memcpy(frame, _inputBuffer.data(), frameSize * sizeof(float));
        
        // Remove processed frame from buffer
        _inputBuffer.erase(_inputBuffer.begin(), _inputBuffer.begin() + frameSize);

        // Process the frame
        ProcessFrame(frame);
        
        // Add to output
        processedFrames.insert(processedFrames.end(), frame, frame + frameSize);
    }

    // Resample back to output rate if needed
    std::vector<float> outputFloat;
    if (outputSampleRate != 48000) {
        outputFloat = Resample(processedFrames.data(), processedFrames.size(), 48000, outputSampleRate);
    } else {
        outputFloat = std::move(processedFrames);
    }

    // Convert back to int16_t
    std::vector<int16_t> result(outputFloat.size());
    Float32ToInt16(outputFloat.data(), result.data(), outputFloat.size());

    return result;
}

