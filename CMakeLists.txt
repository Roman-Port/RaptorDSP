cmake_minimum_required(VERSION 3.10.2)
project("raptordsp")
add_library(raptordsp SHARED
        src/fast_atan2.cpp
        src/analog/pll.cpp
        src/analog/costas_loop.cpp
        src/digital/clock_recovery_mm.cpp
        src/digital/rds/rds_demod.cpp
        src/digital/rds/rds_decode.cpp
        src/filter/fir/filter_complex.cpp
        src/filter/fir/filter_real.cpp
        src/filter/window.cpp
        src/filter/builder/builder.cpp
        src/filter/builder/builder_bandpass.cpp
        src/filter/builder/builder_lowpass.cpp
        src/filter/builder/builder_rootraised.cpp
        src/misc/wav.cpp
)
set_target_properties(raptordsp PROPERTIES PUBLIC_HEADER include)
target_include_directories(raptordsp PUBLIC include)

target_include_directories(raptordsp PRIVATE C:/Users/Roman/Documents/AndroidRadioBuild/toolchain/${ANDROID_ABI}/include/)

# Link VOLK
target_link_libraries(raptordsp volk)