include(FetchContent)

# RtAudio
FetchContent_Declare(
        rtaudio
        GIT_REPOSITORY https://github.com/thestk/rtaudio.git
        GIT_TAG 6.0.1
)

FetchContent_MakeAvailable(rtaudio)

# RNNoise via FetchContent: fetch sources and build a static library with CMake
FetchContent_Declare(
        rnnoise
        GIT_REPOSITORY https://github.com/xiph/rnnoise.git
        GIT_TAG master
)

FetchContent_GetProperties(rnnoise)
if(NOT rnnoise_POPULATED)
    FetchContent_Populate(rnnoise)
endif()

set(RNNOISE_SOURCE_DIR ${rnnoise_SOURCE_DIR})

set(RNNOISE_SOURCES
        ${RNNOISE_SOURCE_DIR}/src/rnn_reader.c
        ${RNNOISE_SOURCE_DIR}/src/denoise.c
        ${RNNOISE_SOURCE_DIR}/src/pitch.c
        # ${RNNOISE_SOURCE_DIR}/src/fft.c
        ${RNNOISE_SOURCE_DIR}/src/rnn.c
        ${RNNOISE_SOURCE_DIR}/src/rnn_data.c
        ${RNNOISE_SOURCE_DIR}/src/celt_lpc.c
        ${RNNOISE_SOURCE_DIR}/src/kiss_fft.c
        # ${RNNOISE_SOURCE_DIR}/src/kiss_fftr.c
        # ${RNNOISE_SOURCE_DIR}/src/detect.c
)

add_library(rnnoise STATIC ${RNNOISE_SOURCES})
target_include_directories(rnnoise
        PUBLIC
        ${RNNOISE_SOURCE_DIR}/include
)

add_library(audio_recorder 
    AudioRecorder.cpp 
    AudioRecorder.hpp
    NoiseSuppressor.cpp
    NoiseSuppressor.hpp
)
message("!!!!!!!")
message(${rtaudio_SOURCE_DIR})
target_include_directories(audio_recorder
        PUBLIC
        ${rtaudio_SOURCE_DIR}/
        ${RNNOISE_SOURCE_DIR}/include
)

target_link_libraries(audio_recorder
        PUBLIC
        rtaudio
        saving_worker
        rnnoise
)


# Фреймворки macOS
find_library(AUDIO_TOOLBOX AudioToolbox)
find_library(CORE_AUDIO CoreAudio)
find_library(CORE_FOUNDATION CoreFoundation)
find_library(CORE_SERVICES CoreServices)
find_library(AUDIO_UNIT AudioUnit)

target_link_libraries(audio_recorder PUBLIC
        ${AUDIO_TOOLBOX}
        ${CORE_AUDIO}
        ${CORE_FOUNDATION}
        ${CORE_SERVICES}
        ${AUDIO_UNIT}
)

set_target_properties(audio_recorder PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)