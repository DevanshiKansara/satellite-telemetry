cmake_minimum_required(VERSION 3.20)
project(satellite_telemetry CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_library(tlm_core onboard/packet.cpp onboard/anomaly.cpp)
target_include_directories(tlm_core PUBLIC onboard)

add_executable(onboard onboard/main.cpp onboard/udp.cpp)
target_link_libraries(onboard PRIVATE tlm_core)

enable_testing()
find_package(GTest REQUIRED)
add_executable(test_core tests/test_core.cpp)
target_link_libraries(test_core PRIVATE tlm_core GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(test_core)
