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
    // Record without suppression
    {
        std::cout << "Recording without noise suppression (rec_raw.wav)..." << std::endl;
        AudioRecorder rawRecorder(std::make_shared<WavWorker>("rec_raw.wav"));
        rawRecorder.SetNoiseSuppressionEnabled(false);
        rawRecorder.Record(5);
        rawRecorder.SaveData();
    }

    // Record with suppression
    {
        std::cout << "Recording with noise suppression (rec_denoised.wav)..." << std::endl;
        AudioRecorder denoisedRecorder(std::make_shared<WavWorker>("rec_denoised.wav"));
        denoisedRecorder.SetNoiseSuppressionEnabled(true);
        denoisedRecorder.Record(5);
        denoisedRecorder.SaveData();
    }

    std::cout << "Done. Compare rec_raw.wav and rec_denoised.wav." << std::endl;
    return 0;
}