#include "../AudioRecorder/RecordData.hpp"
#include "../SavingWorkers/WavWorker.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <atomic>
#include <thread>

#include "../AudioRecorder/AudioRecorder.hpp"

int main()
{
    AudioRecorder recorder(std::make_shared<WavWorker>("rec.wav"));
    recorder.Record(5);
    recorder.SaveData();
    return 0;
}