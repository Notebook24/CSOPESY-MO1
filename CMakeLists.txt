cmake_minimum_required(VERSION 3.10)
project(csopesy LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(csopesy ${SOURCES})
target_include_directories(csopesy PRIVATE src)
target_compile_features(csopesy PRIVATE cxx_std_17)
find_package(Threads REQUIRED)
target_link_libraries(csopesy PRIVATE Threads::Threads)

# copy config.txt next to the executable after configuring
configure_file(${CMAKE_SOURCE_DIR}/config.txt ${CMAKE_BINARY_DIR}/config.txt COPYONLY)