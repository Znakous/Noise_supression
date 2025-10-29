#pragma once
#include <vector>
#include <atomic>

struct RecordData {
    std::vector<int16_t> audioData;
    std::atomic<bool> isRecording;
    unsigned int sampleRate;
};
