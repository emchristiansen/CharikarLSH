cmake_minimum_required(VERSION 2.8.7)

project(CharikarLSH)

# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
# set(CMAKE_CXX_FLAGS "-Wno-c++11-extensions")

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "-std=c++0x -O3")

#set(CMAKE_CXX_FLAGS "-O3 -march=native -msse4.2 -std=c++0x")
#set(CMAKE_CXX_FLAGS "-std=c++0x")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main ${OpenCV_LIBS} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})