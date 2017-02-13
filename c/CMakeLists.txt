cmake_minimum_required(VERSION 3.6)
project(B_problem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
# Add OpenCV headers location to your include paths
include_directories(${OpenCV_INCLUDE_DIRS})


set(SOURCE_FILES main.cpp model.cpp model.h)
add_executable(B_problem ${SOURCE_FILES})
target_link_libraries(B_problem ${OpenCV_LIBS})