cmake_minimum_required(VERSION 3.15)

project(Netprogram C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})


add_subdirectory(lib/)
add_subdirectory(src/ch1)
add_subdirectory(src/ch2)
add_subdirectory(src/ch3)
add_subdirectory(src/ch4)
add_subdirectory(src/ch5)
add_subdirectory(src/ch6)
add_subdirectory(src/ch7)
add_subdirectory(src/ch8)
add_subdirectory(src/ch9)
add_subdirectory(src/ch10)
add_subdirectory(src/mid_homework)
add_subdirectory(src/ch11)
add_subdirectory(src/ch12)
add_subdirectory(src/ch13)