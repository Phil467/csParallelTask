cmake_minimum_required(VERSION 3.10)
project(csParallelTask VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(csParallelTask SHARED ${SOURCES})

add_executable(csParallelTaskTest test/MatrixMult_Perf_Compare.cpp)
target_link_libraries(csParallelTaskTest PRIVATE csParallelTask)

install(TARGETS csParallelTask DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

include(GNUInstallDirs)