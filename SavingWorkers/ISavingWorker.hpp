#pragma once

#include <stdexcept>
#include <vector>
#include <cstdint>

struct SavingWorkerException : public std::runtime_error {
    SavingWorkerException(const std::string& what) : std::runtime_error(what) {}
};

class ISavingWorker {
public:
    ISavingWorker() : _setter_called(false) {}
    virtual ~ISavingWorker() = default;
    virtual bool Save() = 0;
    void SetSampleRate();
    void SetAudioData(const std::vector<int16_t>& audioData, unsigned int sampleRate);
protected:
    const int16_t* _audioData;
    size_t _audioDataSize;
    unsigned int _sampleRate;
    bool _setter_called;
};