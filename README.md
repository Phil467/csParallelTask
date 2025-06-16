# csParallelTask Library

## Overview

The csParallelTask library is a powerful and flexible parallelization solution for C++ that simplifies leveraging modern multi-core architectures. It provides a structured framework for dividing intensive tasks into work blocks that can be executed simultaneously across different threads.

## Key Features

- 🧩 **Simplified Parallelization**: Easily transform sequential operations into parallel processes without directly managing thread complexities.
- 🔄 **Automatic Adaptation**: Dynamically adjusts to the number of available cores on the machine for optimal resource utilization.
- 📦 **Flexible Argument Management**: Advanced mechanisms for passing and sharing data between worker threads.
- ⏱️ **Integrated Performance Measurements**: Precise timing tools to evaluate performance gains.
- 🎮 **Execution Control**: Options for synchronous or asynchronous (background) executions.

## Complex Tasks Optimized by csParallelTask

1. 🖼️ **High-Resolution Image Processing**: Divide an image into sections and apply filters or transformations in parallel, significantly reducing processing time.

2. 🔬 **Scientific Simulations**: Distribute intensive numerical calculations across multiple cores to accelerate physical, chemical, or other complex mathematical models.

3. 📊 **Large Data Analysis**: Process big datasets in parallel, with automatic load distribution among available threads.

4. 🔍 **Search and Sorting Algorithms**: Efficiently implement parallel search or sorting algorithms on large data collections.

5. 🧮 **Matrix Computations**: Perform operations on large matrices by dividing the work into blocks for optimal performance.

6. 🎨 **Graphical Rendering**: Accelerate 3D rendering calculations by parallelizing operations by zones or objects.

7. 🗜️ **Data Compression/Decompression**: Process multiple data segments simultaneously to improve compression algorithm performance.

8. 🌳 **Tree Traversals**: Explore complex tree structures in parallel for applications.

## Core Components

### csPARGS
Manages arguments and boundaries for parallel work blocks, providing a clean interface for thread communication.

### csPERF_MEASUREMENT
Offers precise timing capabilities from nanoseconds to hours to measure and optimize parallel execution performance.

### csParallelTask Namespace
Contains the main functionality for creating, managing, and executing parallel tasks with flexible control over execution parameters.

## Benefits

The csParallelTask library eliminates the usual complexity of parallel programming by providing high-level abstractions that allow developers to focus on their business logic rather than thread implementation details. With its intuitive API and optimized performance, it represents an ideal solution for modern applications requiring efficient use of multi-core computing resources.

## Usage Example

```cpp
// Register a parallel function with regular work distribution
size_t taskId = csParallelTask::registerFunctionRegularEx(
    8,                  // Number of blocks/threads (adapts to CPU cores)
    dataSize,           // Total work size to process
    "processData",      // Task name
    &processDataFunc,   // Function pointer
    2,                  // Number of arguments
    &inputData,         // Arg 1: Input data pointer
    &results            // Arg 2: Results container pointer
);

// Execute the parallel task
csParallelTask::execute(taskId);
```

## Performance Measurement

```cpp
// Measure execution time
csPERF_MEASUREMENT timer(CSTIME_UNIT_MILLISECOND);
timer.start();

// Execute parallel processing
csParallelTask::execute("processData");

timer.stop();
timer.printReport("Data processing completed in: ");
```

---

## ⚙️ Build Instructions

### 🔷 Windows (MSYS2 + g++)
```bash
pacman -S mingw-w64-x86_64-toolchain cmake
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make
```
### 🔷 Windows (MSYS2 CLANG64)
```bash
pacman -S mingw-w64-clang-x86_64-toolchain cmake ninja
mkdir build && cd build
cmake .. -G Ninja
ninja
```
### 🐧 Linux / 🍎 macOS
```bash
sudo apt install g++ cmake ninja-build   # Or use clang++
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```


## 📁 Directory Structure

```bash
csParallelTask/
├── include/               # Public headers
│   ├── csPARALLEL.h
│   ├── csPARGS.h
│   └── csPERF_MEASUREMENT.h
├── src/                   # Source files
├── test/                  # Optional test programs
├── build/                 # Out-of-source build directory
├── README.md
└── LICENSE
```

## 📜 License
This project is licensed under the MIT License. See the LICENSE file for details.

## ✉️ Contributions
Feedback, issues, and pull requests are welcome.
This library is part of the CSigma initiative to empower developers with efficient and scientifically grounded C++ tools.
