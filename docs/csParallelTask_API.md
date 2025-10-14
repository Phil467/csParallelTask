# CSigma — Parallel Task Module Documentation

**Version 1.0 — Copyright Phil467, 2025**

This document provides an API-style reference for the `csParallelTask` component of the CSigma project. It reproduces function prototypes and their comments exactly as they appear in the header files, organised for easy reading on GitHub.

---

## Table of Contents
- [csPARALLEL.h](#csparallelh)
  - [Namespace `csParallelTask` overview](#namespace-csparalleltask-overview)
  - [Templates & Utility](#templates--utility)
  - [Functions](#functions)
- [csPARGS.h](#cspargsh)
  - [Class `csPARGS`](#class-cspargs)
  - [Methods & Operators](#methods--operators)
- [csPERF_MEASUREMENT.h](#csperf_measurementh)
  - [Class `csPERF_MEASUREMENT`](#class-csperf_measurement)
  - [Methods](#methods-1)

---

## csPARALLEL.h

```cpp
#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CSPARALLEL_API __declspec(dllexport)
  #else
    #define CSPARALLEL_API __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CSPARALLEL_API __attribute__ ((visibility ("default")))
  #else
    #define CSPARALLEL_API
  #endif
#endif

#ifndef CSPARALLEL_H_INCLUDED
#define CSPARALLEL_H_INCLUDED

#include <cstddef>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <string.h>
#include <memoryapi.h>
#include "csPARGS.h"
#include "csPERF_MEASUREMENT.h"

using namespace std;

typedef csPARGS::BOUNDS* BUFFER_SHAPE;
```
### Namespace `csParallelTask` overview

The header declares the `csParallelTask` namespace containing utilities to register functions, set up arguments and buffer shapes and execute tasks in parallel.

---
### Templates & Utility

#### `registerArgs` (variadic template)
```cpp
/**
 * @brief TODO: register arguments for some function (a least one argument).
 * @param Args TODO: Returned tqable that will contain registered arguments.
 * @param nbArgs TODO: Number of arguments.
 * @param arg TODO: Firts argument.
 * @param args TODO: Other arguments.
 */
template<typename... _Args> void registerArgs(void**& Args, size_t& nbArgs, void* arg, _Args... args)
```
```cpp
inline void registerArgs(void**& Args, size_t& nbArgs)
```
---

### Functions

> The following functions are declared in `csPARALLEL.h`. Each entry shows the prototype followed by the original comment block preserved exactly.

#### `void setArgs(size_t idf, BUFFER_SHAPE shape, csPARGS funcArgs)`
```cpp
/**
 * @brief TODO: Sets up arguments of each work block for the function indexed by 'idf'.
 * @param idf TODO: Index of the function.
 * @param shape TODO: Table containing the bounds of each created block.
 * @param funcArgs TODO: Object of csPARGS class containing arguments of the function.
 */
void setArgs(size_t idf, BUFFER_SHAPE shape, csPARGS funcArgs);
```
#### `void setArgsRegular(size_t idf, size_t workSize, csPARGS funcArgs)`
```cpp
/**
 * @brief TODO: Sets up arguments of each regular (same size) work block for the function indexed by 'idf'.
 * @param idf TODO: Index of the function.
 * @param workSize TODO: The size of the work, used to build regular blocks.
 * @param funcArgs TODO: Object of csPARGS class containing arguments of the function.
 */
void setArgsRegular(size_t idf, size_t workSize, csPARGS funcArgs);
```
#### `void setBufferShape(size_t idf, BUFFER_SHAPE shape)`
```cpp
/**
 * @brief TODO: reshape the work by assigning new one.
 * @param idf TODO: Index of the function.
 * @param shape TODO: Table containing the bounds of each created block.
 */
void setBufferShape(size_t idf, BUFFER_SHAPE shape);
```
#### `void setBufferShapeRegular(size_t idf, size_t workSize)`
```cpp
/**
 * @brief TODO: reshape the work with regular (same size) blocks, following the new 'workSize' parameter.
 * @param idf TODO: Index of the function.
 * @param workSize TODO: Size of the work.
 */
void setBufferShapeRegular(size_t idf, size_t workSize);
```
#### `void setDelay(size_t idf, size_t delay)`
```cpp
/**
 * @brief TODO: Sets up time delay (in nanoseconds) for loops to make safe execution.
 * @param idf TODO: Index of the function.
 * @param delay TODO: Delay to assign to every thread.
 */
void setDelay(size_t idf, size_t delay);
```
#### `void setDelay(size_t idf, vector<size_t> delayList)`
```cpp
/**
 * @brief TODO: Sets up time delay (in nanoseconds) for loops to make safe execution.
 * @param idf TODO: Index of the function.
 * @param delayList TODO: Lists of delay values for each thread.
 */
void setDelay(size_t idf, vector<size_t> delayList);
```
#### `void setExecutionMode(size_t idf, bool execMode)`
```cpp
/**
 * @brief TODO: Defines weather the function will be executed normally or in background.
 * @param idf TODO: Index of the function.
 * @param execMode TODO: Execution mode. Can be : CSTHREAD_NORMAL_EXECUTION or CSTHREAD_BACKGROUND_EXECUTION.
 */
void setExecutionMode(size_t idf, bool execMode);
```
#### `void setExecutionMode(size_t idf, vector<bool> execMode)`
```cpp
/**
 * @brief TODO: Defines weather each thread will be executed normally or in background.
 * @param idf TODO: Index of the function.
 * @param execMode TODO: List of execution mode for each thread. Each one can be : CSTHREAD_NORMAL_EXECUTION or CSTHREAD_BACKGROUND_EXECUTION.
 */
void setExecutionMode(size_t idf, vector<bool> execMode);
```
#### `BUFFER_SHAPE makeRegularBufferShape(size_t workSize, size_t& nBlocks)`
```cpp
/**
 * @brief TODO: Make 'nBlocks' Threads of same size that work each one on a part of the work.
 * @param workSize TODO: The size of the work.
 * @param nBlocks TODO: Number of work blocks. Each block is managed by one thread. 'nBlocks' is modified to be equal to the number of computing units of the CPU, if it value is greater.
 * @return BUFFER_SHAPE  TODO: Table containing all the created blocks.
 */
BUFFER_SHAPE  makeRegularBufferShape(size_t workSize, size_t& nBlocks);
```
#### `size_t registerFunction(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char* fName, void(*blockFunc)(csPARGS), csPARGS funcArgs)`
```cpp
/**
 * @brief TODO: Register a new function.
 * @param nBlocks TODO: Number of work blocks, corresponding each one to a thread.
 * @return workSize TODO: Buffer Size.
 * @param shape TODO: Table containing the bounds of each block to be created.
 * @param charfName TODO: Name of the function to register.
 * @param blockFunc TODO: Pointer to the function to register.
 * @return size_t TODO: Index of the registered function.
 */
size_t registerFunction(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char* fName, void(*blockFunc)(csPARGS), csPARGS funcArgs);
```
#### `size_t registerFunctionEx(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char*fName, void(*blockFunc)(csPARGS), size_t nbArgs,...)`
```cpp
/**
 * @brief TODO: Extended registation of a new function. Allows to specify directly pointers to each argument of the function, rather than giving a 'csPARGS' class that contains arguments, as done by 'registerFunction'.
 * @param nBlocks TODO: Number of work blocks, corresponding each one to a thread.
 * @return workSize TODO: Buffer Size.
 * @param shape TODO: Table containing the bounds of each block to be created.
 * @param charfName TODO: Name of the function to register.
 * @param blockFunc TODO: Pointer to the function to register.
 * @return size_t TODO: Index of the registered function.
 */
size_t registerFunctionEx(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char*fName, void(*blockFunc)(csPARGS), size_t nbArgs,...);
```
#### `size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, char*fName, void(*blockFunc)(csPARGS), size_t nbArgs,...)`
```cpp
/**
 * @brief TODO: Extended registation of a new function. Allows to specify directly pointers to each argument of the function, rather than giving a 'csPARGS' class that contains arguments, as done by 'registerFunction'.
 * @param nBlocks TODO: Number of work blocks, corresponding each one to a thread.
 * @param workSize TODO: The size of the work, used to build regular blocks.
 * @param charfName TODO: Name of the function to register.
 * @param blockFunc TODO: Pointer to the function to register.
 * @return size_t TODO: Index of the registered function.
 */
size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, char*fName, void(*blockFunc)(csPARGS), size_t nbArgs,...);
```
#### Template overload for `registerFunctionRegularEx` (variadic)
```cpp
/**
 * @brief TODO: Extended registation of a new function. Allows to specify directly pointers to each argument of the function, rather than giving a 'csPARGS' class that contains arguments, as done by 'registerFunction'.
 * @param nBlocks TODO: Number of work blocks, corresponding each one to a thread.
 * @param workSize TODO: The size of the work, used to build regular blocks.
 * @param charfName TODO: Name of the function to register.
 * @param blockFunc TODO: Pointer to the function to register.
 * @param arg TODO: Void Pointer to the first argument.
 * @param args TODO: Other arguments in variable number.
 */
template<typename... _Args> size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, char*fName, void(*Function)(csPARGS), void*arg, _Args... args)
```
#### `size_t getRightThreadNumber(size_t nThread)`
```cpp
/**
 * @brief TODO: Returns min(nThread, getMaxThreadNumber()), where 'getMaxThreadNumber()' function gives the number of physical units of the CPU.
 * @param nThread TODO: Number of thread needed.
 * @return size_t TODO: min(nThread, getMaxThreadNumber()).
 */
size_t getRightThreadNumber(size_t nThread);
```
#### `size_t getMaxThreadNumber()`
```cpp
/**
 * @brief TODO: Gives the number of physical units of the CPU. Each one excecutes one thread.
 * @return size_t TODO: Number of physical units of the CPU.
 */
size_t getMaxThreadNumber();
```
#### `vector<csPARGS> getArgs(size_t idf)`
```cpp
/**
 * @brief TODO: Returns table that contains the 'csPARGS' arguments objects of all the blocks.
 * @param idf TODO: Index of the function.
 * @return vector<csPARGS> TODO: Table of the arguments objects of all blocks.
 */
vector<csPARGS> getArgs(size_t idf);
```
#### `csPARGS getArgs(size_t idf, size_t ida)`
```cpp
/**
 * @brief TODO: Returns the arguments object of the 'ida' block for the 'idf' function.
 * @param idf TODO: Index of the function.
 * @param ida TODO: Index of block.
 * @return csPARGS TODO: Arguments object of 'ida' block.
 */
csPARGS getArgs(size_t idf, size_t ida);
```
#### `size_t getId(const char* funcName)`
```cpp
/**
 * @brief TODO: Returns the index of the function identified by it name 'funcName'.
 * @return size_t TODO: Index of the specified function.
 */
size_t getId(const char*funcName);
```
#### `size_t getId(void(*f)(csPARGS))`
```cpp
/**
 * @brief TODO: Returns the index of the function identified by it name address 'f'.
 * @param f TODO: Address of the function.
 * @return size_t TODO: Index of the specified function.
 */
size_t getId(void(*f)(csPARGS));
```
#### `size_t getWorkSize(int idf)`
```cpp
/**
 * @brief TODO: Returns the global work size for the function indexed by 'idf'.
 * @param idf TODO: Index of the function.
 * @return size_t TODO: Global work size.
 */
size_t getWorkSize(int idf);
```
#### `void updateArg(size_t idf, size_t ida, void* &&arg)`
```cpp
/**
 * @brief TODO: Update the 'ida' argument of 'idf' function, with 'arg'.
 * @param idf TODO: Index of the function.
 * @param ida TODO: Index of the argument.
 * @param arg TODO: Argument to assign.
 */
void updateArg(size_t idf, size_t ida, void* &&arg);
```
#### `void updateArg(size_t idf, initializer_list<size_t> ida, initializer_list<void*> arg)`
```cpp
/**
 * @brief TODO: Update the 'ida' list of arguments of 'idf' function, with 'arg' list.
 * @param idf TODO: Index of the function.
 * @param ida TODO: List of arguments Indexes.
 * @param arg TODO: Lists of arguments to assign.
 */
void updateArg(size_t idf, initializer_list<size_t> ida, initializer_list<void*> arg);
```
#### `template<size_t N> void updateArg(size_t idf, const size_t (&ida)[N], void* const (&arg)[N])`
```cpp
/**
* @brief TODO: Update the 'ida' list of arguments of 'idf' function, with 'arg' list.
 * @param idf TODO: Index of the function.
 * @param ida TODO: List of arguments Indexes.
 * @param arg TODO: Lists of arguments to assign.
 */
template<size_t N> void updateArg(size_t idf, const size_t (&ida)[N], void* const (&arg)[N]);
```
#### `void execute(int id)`
```cpp
/**
 * @brief TODO: Computes every block in parallel.
 * @param id TODO: Index of the function to execute.
 */
void execute(int id);
```
#### `void execute(const char* funcName)`
```cpp
/**
 * @brief TODO: Computes every block in parallel.
 * @param charfuncName TODO: Name of the function to execute.
 */
void execute(const char*funcName);
```
#### `void execute(void(*f)(csPARGS))`
```cpp
/**
 * @brief TODO: Computes every block in parallel for the function specified by pointer 'f'.
 * @param f TODO: Pointer to the function to execute.
 */
void execute(void(*f)(csPARGS));
```
#### `void execute(vector<thread> threads)`
```cpp
/**
 * @brief TODO: Describe the purpose of execute.
 * @param threads TODO: Table Threads to execute.
 */
void execute(vector<thread> threads);
```

End of `csPARALLEL.h` section.
---

## csPARGS.h

```cpp
#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CSPARALLEL_API __declspec(dllexport)
  #else
    #define CSPARALLEL_API __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CSPARALLEL_API __attribute__ ((visibility ("default")))
  #else
    #define CSPARALLEL_API
  #endif
#endif

#ifndef CSPARGS_H_INCLUDED
#define CSPARGS_H_INCLUDED

#include <iostream>
#include <stdlib.h>
#include <cstdarg>
#include <thread>
#include <mutex>

#define CSTHREAD_NORMAL_EXECUTION 0
#define CSTHREAD_BACKGROUND_EXECUTION 1


template<class T> T* _csAlloc(size_t n)
{
  T*t = (T*)malloc(n*sizeof(T));
  return t;
}

template<class T> T* _csAlloc(size_t n, T init)
{
  T*t = (T*)malloc(n*sizeof(T));
  for(size_t i=0; i<n; i++)
  {
    t[i] = init;
  }
  return t;
}

class csPARGS
{
public:

    typedef struct
    {
        size_t i;
        void*arg;
    }csVOID_ARG;

    typedef struct
    {
      size_t first;
      size_t last;
    }BOUNDS;

    csPARGS(size_t nArgs=0);
```
### Class `csPARGS` — Methods & Descriptions

> Methods are presented with prototypes and original comment blocks preserved exactly.

#### `void init(size_t _nbArgs)`
```cpp
/**
 * @brief TODO: Initializes an Object.
 * @param _nbArgs TODO: Number of arguments.
 */
void init(size_t _nbArgs);
```
#### `void setArg(size_t i, void* arg)`
```cpp
/**
 * @brief TODO: Sets up the argument indexed by 'i'.
 * @param i TODO: Index of the argument.
 * @param arg TODO: Void pointer containing the argument value to assign.
 */
void  setArg(size_t i, void* arg);
```
#### `void setArgNumber(size_t nbArgs)`
```cpp
/**
 * @brief TODO: Sets up the number of arguments.
 * @param nbArgs TODO: Number of arguments.
 */
void setArgNumber(size_t nbArgs);
```
#### `void setBounds(csPARGS::BOUNDS bounds)`
```cpp
/**
 * @brief TODO: Sets up the work block bounds.
 * @param bounds TODO: Strucure 'csPARGS::BOUNDS' that contains the bounds of the work block.
 */
void setBounds(csPARGS::BOUNDS bounds);
```
#### `void setBlockId(size_t id)`
```cpp
/**
 * @brief TODO: Saves the current block index;
 * @param id TODO: Index of the block.
 */
void setBlockId(size_t id);
```
#### `void setBlocksNumber(size_t blockNumber)`
```cpp
/**
 * @brief TODO: Saves the number of work blocks created.
 * @param blockNumber TODO: nNumber of work blocks created.
 */
void setBlocksNumber(size_t blockNumber);
```
#### `void setWorkSize(size_t workSize)`
```cpp
/**
 * @brief TODO: Sets up the work size.
 * @param workSize TODO: Work size.
 */
void setWorkSize(size_t workSize);
```
#### `void setDelay(size_t delay)`
```cpp
/**
 * @brief TODO: Sets up time delay in the thread.
 * @param delay TODO: Time delay.
 */
void setDelay(size_t delay);
```
#### Sleep helper methods
```cpp
/**
 * @brief TODO: Applies an hours delay in the thread.
 */
void sleepHour();
/**
 * @brief TODO: Applies a minutes delay in the thread.
 */
void sleepMin();
/**
 * @brief TODO: Applies a seconds delay in the thread.
 */
void sleepSec();
/**
 * @brief TODO: Applies a milliseconds delay in the thread.
 */
void sleepMilli();
/**
 * @brief TODO: Applies a microseconds delay in the thread.
 */
void sleepMicro();
/**
 * @brief TODO: Applies a nanoseconds delay in the thread.
 */
void sleepNano();
```
#### `void* getArg(size_t i)`
```cpp
/**
 * @brief TODO: Returns the argument indexed by 'i'.
 * @param i TODO: Index of the argument.
 * @return void* TODO: Void pointer containing the argument.
 */
void* getArg(size_t i);
```
#### `size_t getBlockId()`
```cpp
/**
 * @brief TODO: Returns the current block index;
 * @return size_t TODO: Current block index;.
 */
size_t getBlockId();
```
#### `size_t getBlocksNumber()`
```cpp
/**
 * @brief TODO: Returns the number of work blocks created.
 * @return size_t TODO: Number of work blocks created.
 */
size_t getBlocksNumber();
```
#### `csPARGS::BOUNDS getBounds()`
```cpp
/**
 * @brief TODO: Returns the work block position;
 * @return csPARGS::BOUNDS TODO: Strucure 'csPARGS::BOUNDS' that contains the bounds of the work block.
 */
csPARGS::BOUNDS getBounds();
```
#### `void getBounds(size_t workSize, size_t*min, size_t*max)`
```cpp
/**
 * @brief TODO: Calculates the regular bounds of a work block following the 'workSize' parameter.
 * @param workSize TODO: Global size.
 * @param min TODO: Output lower bound.
 * @param max TODO: Output upper bound.
 */
void getBounds(size_t workSize, size_t*min, size_t*max);
```
#### `size_t getArgNumber()`
```cpp
/**
 * @brief TODO: Returns the Number of registered arguments.
 * @return size_t TODO: Number of registered arguments.
 */
size_t getArgNumber();
```
#### Template `getArgPtr`
```cpp
/**
 * @brief TODO: Returns a pointer to the argument indexed by 'idArg'.
 * @param idArg TODO: Index of the argument.
 * @return T* TODO: Void pointer to the argument.
 */
template <class T> T* getArgPtr(size_t idArg)
{
    return (T*)Args[idArg+2];
};
```
#### `size_t getWorkSize()`
```cpp
/**
 * @brief TODO: Returns the work size.
 * @return size_t TODO: Work size.
 */
size_t getWorkSize();
```
#### `void makeArgs(...)` and `void makeArgs2(void** args, size_t nbArgs)`
```cpp
/**
 * @brief TODO: Sets up arguments.
 * @param ... TODO: arguments.
 */
void  makeArgs(...);
/**
 * @brief TODO: Sets up arguments.
 * @param args TODO: Table of void pointers to arguments addresses.
 * @param nbArgs TODO: Number of arguments.
 */
void  makeArgs2(void** args, size_t nbArgs);
```
#### Template overload `makeArgs2(void* args[_nbArgs])`
```cpp
/**
 * @brief TODO: Sets up arguments.
 * @param args TODO: Table of void pointers to arguments addresses.
 * @param nbArgs TODO: Number of arguments.
 */
template<size_t _nbArgs> void makeArgs2(void*args[_nbArgs]);
```
#### `void lockGuard()`
```cpp
/**
 * @brief TODO: Activates mutex for safe thread coordination when accessing same ressources.
 */
void lockGuard();
```
#### `void clear()`
```cpp
/**
 * @brief TODO: Release object.
 */
void  clear();
```
#### Conversion operators & index operators
```cpp
/**
 * @brief TODO: Returns the boundaries of the work section where current thread is executed.
 * @return csPARGS::BOUNDS TODO: Boundaries of the work section.
 */
operator csPARGS::BOUNDS();
/**
 * @brief TODO: Returns index of the current thread.
 * @return size_t TODO: Index of the current thread.
 */
operator size_t();
/**
 * @brief TODO: Returns a void pointer to the argument indexed by 'i'.
 * @return void* TODO: Void pointer to the argument.
 */
void* operator[](size_t i);
/**
 * @brief TODO: Assign a new void pointer to the argument indexed by the member 'i' of the 'va' object.
 * @return void* TODO: Index of the current thread.
 */
void operator=(csVOID_ARG va);
```
#### `bool EXEC_MODE`
```cpp
/**
 * @brief TODO: Defines weather the thread will be executed normally or in background. Can be : CSTHREAD_NORMAL_EXECUTION or CSTHREAD_BACKGROUND_EXECUTION
 */
bool EXEC_MODE = CSTHREAD_NORMAL_EXECUTION;
```
End of `csPARGS.h` section.
---

## csPERF_MEASUREMENT.h

```cpp
#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CSPARALLEL_API __declspec(dllexport)
  #else
    #define CSPARALLEL_API __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CSPARALLEL_API __attribute__ ((visibility ("default")))
  #else
    #define CSPARALLEL_API
  #endif
#endif

#ifndef CSPERF_MEASUREMENT_H
#define CSPERF_MEASUREMENT_H

#include <chrono>
#include <iostream>

#define CSTIME_UNIT_HOUR            0
#define CSTIME_UNIT_MINUTE          1
#define CSTIME_UNIT_SECOND          2
#define CSTIME_UNIT_MILLISECOND     3
#define CSTIME_UNIT_MICROSECOND     4
#define CSTIME_UNIT_NANOSECOND      5
```
### Class `csPERF_MEASUREMENT` — Overview & Methods

```cpp
class csPERF_MEASUREMENT
{
  private:
  std::chrono::time_point<std::chrono::high_resolution_clock>
  strt, stp;
  size_t ellapsed;
  int unit = CSTIME_UNIT_MICROSECOND;
  char*unitName = " microseconds\0";

  public:
/**
 * @brief TODO: Constructor.
 * @param unit TODO: Time unit.
 */
  csPERF_MEASUREMENT(int unit = CSTIME_UNIT_MICROSECOND);
/**
 * @brief TODO: Sets up the time unit.
 * @param unit TODO: Time unit.
 */
  void setTimeUnit(int unit);
/**
 * @brief TODO: Starts measuring execution time.
 */
  void start();
/**
 * @brief TODO: Stop measuring execution time.
 */
  void stop();
/**
 * @brief TODO: Print the execution time for a code block
 * @param title TODO: Output Title.
 */
  void printReport(const char* title);
/**
 * @brief TODO: Returns execution time for a code block.
 * @return size_t TODO: Execution time.
 */
  size_t getEllapsedTime();
};
```
End of `csPERF_MEASUREMENT.h` section.
---

### Footer & notes

This API-style documentation reproduces the prototypes and comments exactly from the header files and reorganises them for display on GitHub with a clickable Table of Contents and code-first presentation.

---
