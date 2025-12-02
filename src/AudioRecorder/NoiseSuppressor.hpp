#pragma once

#include <vector>
#include <cstdint>
#include <memory>

// Forward declaration - we'll include rnnoise.h in the cpp file
struct DenoiseState;

// Custom deleter for rnnoise's opaque DenoiseState
// Defined in the .cpp where rnnoise.h is included.
struct DenoiseDeleter {
    void operator()(DenoiseState* ptr) const noexcept;
};

class NoiseSuppressor {
public:
    NoiseSuppressor();
    ~NoiseSuppressor();

    // Enable or disable denoising
    void SetEnabled(bool enabled);
    bool IsEnabled() const;

    // Process audio data in real-time
    // Input: int16_t samples at inputSampleRate
    // Output: denoised int16_t samples at outputSampleRate (same as input if no resampling needed)
    // Returns: processed samples
    std::vector<int16_t> ProcessSamples(const int16_t* samples, size_t numSamples, 
                                        unsigned int inputSampleRate, unsigned int outputSampleRate);

private:
    // Convert int16_t to float32 normalized to [-1.0, 1.0]
    void Int16ToFloat32(const int16_t* input, float* output, size_t count);
    
    // Convert float32 normalized to [-1.0, 1.0] to int16_t
    void Float32ToInt16(const float* input, int16_t* output, size_t count);
    
    // Resample from inputRate to outputRate using linear interpolation
    std::vector<float> Resample(const float* input, size_t inputSamples, 
                                unsigned int inputRate, unsigned int outputRate);
    
    // Process a frame of 480 samples (required by rnnoise)
    void ProcessFrame(float* frame);

    std::unique_ptr<DenoiseState, DenoiseDeleter> _denoiseState;
    bool _enabled;
    
    // Buffer for accumulating samples until we have a full frame (480 samples at 48kHz)
    std::vector<float> _inputBuffer;
    unsigned int _currentInputRate;
    unsigned int _currentOutputRate;
};


