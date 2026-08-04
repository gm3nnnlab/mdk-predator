cmake_minimum_required(VERSION 3.16)

project(mdk_predator LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(mdk_predator STATIC
    hal/hal.c
    src/mdk_predator.c
    src/automotive/key_fob_analyzer.c
    src/automotive/rolling_code_tester.c
    src/wireless/wifi_analyzer.c
    src/wireless/bluetooth_analyzer.c
    src/wireless/subghz_analyzer.c
    src/crypto/crypto_analyzer.c
)

target_include_directories(mdk_predator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(mdk_predator PRIVATE /W4)
else()
    target_compile_options(mdk_predator PRIVATE -Wall -Wextra)
endif()

enable_testing()
