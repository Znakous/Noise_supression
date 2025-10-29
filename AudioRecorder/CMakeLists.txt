include(FetchContent)

# RtAudio
FetchContent_Declare(
        rtaudio
        GIT_REPOSITORY https://github.com/thestk/rtaudio.git
        GIT_TAG 6.0.1
)

FetchContent_MakeAvailable(rtaudio)

add_library(audio_recorder AudioRecorder.cpp AudioRecorder.hpp)
message("!!!!!!!")
message(${rtaudio_SOURCE_DIR})
target_include_directories(audio_recorder
        PUBLIC
        ${rtaudio_SOURCE_DIR}/
)

target_link_libraries(audio_recorder
        PUBLIC
        rtaudio
        saving_worker
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