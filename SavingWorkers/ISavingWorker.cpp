#include "ISavingWorker.hpp"

#include <string>
#include <cstdint>

void ISavingWorker::SetAudioData(const std::vector<int16_t>& audioData, unsigned int sampleRate) {
    _sampleRate = sampleRate;
    _audioData = audioData.data();
    _audioDataSize = audioData.size();
    _sampleRate = sampleRate;
    _setter_called = true;
}

