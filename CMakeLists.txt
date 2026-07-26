cmake_minimum_required(VERSION 3.16)
project(mdk_predator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/hal
)

# Compiler flags
if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Platform-specific math library
if(NOT WIN32)
    set(MATH_LIB m)
endif()

# Core source files
set(AUTOMOTIVE_SOURCES
    src/automotive/key_fob_analyzer.c
    src/automotive/rolling_code_tester.c
)

set(WIRELESS_SOURCES
    src/wireless/wifi_analyzer.c
    src/wireless/bluetooth_analyzer.c
    src/wireless/subghz_analyzer.c
)

set(CRYPTO_SOURCES
    src/crypto/crypto_analyzer.c
)

set(HAL_SOURCES
    hal/hal.c
)

set(CORE_SOURCES
    src/mdk_predator.c
)

# Build the core library
add_library(mdk_predator_lib STATIC
    ${AUTOMOTIVE_SOURCES}
    ${WIRELESS_SOURCES}
    ${CRYPTO_SOURCES}
    ${HAL_SOURCES}
    ${CORE_SOURCES}
)
target_link_libraries(mdk_predator_lib ${MATH_LIB})

# Enable CTest
enable_testing()

# Helper function to add a test executable
function(add_mdk_test target_name source_file)
    add_executable(${target_name} ${source_file} ${ARGN})
    target_link_libraries(${target_name} ${MATH_LIB})
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

# Automotive tests
add_mdk_test(test_key_fob_analyzer
    tests/automotive/test_key_fob_analyzer.c
    ${AUTOMOTIVE_SOURCES}
)

add_mdk_test(test_rolling_code_tester
    tests/automotive/test_rolling_code_tester.c
    ${AUTOMOTIVE_SOURCES}
)

# Wireless tests
add_mdk_test(test_bluetooth_analyzer
    tests/wireless/test_bluetooth_analyzer.c
    ${WIRELESS_SOURCES}
)

add_mdk_test(test_subghz_analyzer
    tests/wireless/test_subghz_analyzer.c
    ${WIRELESS_SOURCES}
)

add_mdk_test(test_wifi_analyzer
    tests/wireless/test_wifi_analyzer.c
    ${WIRELESS_SOURCES}
)

# Crypto tests
add_mdk_test(test_crypto_analyzer
    tests/crypto/test_crypto_analyzer.c
    ${CRYPTO_SOURCES}
)

# Integration tests
# test_input_validation only uses inline functions from input_validation.h,
# so no additional source files are required.
add_executable(test_input_validation
    tests/integration/test_input_validation.c
)
add_test(NAME test_input_validation COMMAND test_input_validation)

add_mdk_test(test_mdk_integration
    tests/integration/test_mdk_integration.c
    ${AUTOMOTIVE_SOURCES}
    ${WIRELESS_SOURCES}
    ${CRYPTO_SOURCES}
    ${HAL_SOURCES}
    ${CORE_SOURCES}
)
