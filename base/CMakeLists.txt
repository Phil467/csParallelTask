cmake_minimum_required(VERSION 3.10)
project(csParallelTask LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DBUILDING_CSPARALLEL_DLL)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# bibliotheque statique
add_library(csParallelTask SHARED src/csParallel.cpp src/csPerfChecker.cpp src/csPargs.cpp)
target_include_directories(csParallelTask PUBLIC include)

# executable
# add_executable(csParallelTask_test src/main.cpp)
# target_link_libraries(csParallelTask_test PRIVATE csParallelTask)
