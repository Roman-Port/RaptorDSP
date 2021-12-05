cmake_minimum_required(VERSION 3.10.2)
project("raptordsp")
set(installhelper_version 0.1.0)
set(WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(volk CONFIG REQUIRED)

add_library(raptordsp SHARED
        src/fast_atan2.cpp
        src/analog/pll.cpp
        src/analog/costas_loop.cpp
        src/digital/clock_recovery_mm.cpp
        src/digital/rds/rds_demod.cpp
        src/digital/rds/rds_decode.cpp
        src/filter/window.cpp
        src/filter/builder/builder.cpp
        src/filter/builder/builder_bandpass.cpp
        src/filter/builder/builder_lowpass.cpp
        src/filter/builder/builder_rootraised.cpp
        src/misc/wav.cpp
 "include/raptordsp/demodulator/demod_bcast_fm.h" "src/demodulator/demod_bcast_fm.cpp" "include/raptordsp/demodulator/demod_bcast_fm_stereo.h" "src/demodulator/demod_bcast_fm_stereo.cpp" "include/raptordsp/demodulator/demod.h" "include/raptordsp/filter/fir/filter_base.h" "src/filter/fir/filter_base.cpp" "src/filter/fir/filter_specializations.cpp" "include/raptordsp/filter/filter_taps.h" "include/raptordsp/digital/clock_recovery_mm_complex.h" "src/digital/clock_recovery_mm_complex.cpp" "include/raptordsp/misc/consumption_queue.h" "src/misc/consumption_queue.cpp" "include/raptordsp/analog/agc.h" "src/analog/agc.cpp" "include/raptordsp/io/wav_reader.h" "src/io/wav_reader.cpp" "include/raptordsp/io/sample_converter.h" "src/io/sample_converter.cpp")
#target_include_directories (raptordsp PUBLIC include)
target_include_directories(raptordsp PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

# Link VOLK
target_link_libraries(raptordsp Volk::volk)

# Set up install
set(installhelper_name raptordsp)
include(install_helper.cmake)
