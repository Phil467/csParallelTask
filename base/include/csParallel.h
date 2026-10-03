#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CS_PARALLEL_TASK_API __declspec(dllexport)
  #else
    #define CS_PARALLEL_TASK_API __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_CSPARALLEL_DLL
    #define CS_PARALLEL_TASK_API __attribute__ ((visibility ("default")))
  #else
    #define CS_PARALLEL_TASK_API
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
#include "CSPARGS.h"
#include "csPerfChecker.h"

using namespace std;

typedef CSPARGS::BOUNDS* BUFFER_SHAPE;

namespace csParallelTask
{

inline void registerArgs(void**&Args, size_t& nbArgs)
{

};

template<typename... _Args> void registerArgs(void**&Args, size_t& nbArgs, void* arg, _Args... args)
{
  nbArgs++;
  Args = (void**)realloc(Args, nbArgs*sizeof(void*));
  Args[nbArgs-1] = arg;
  registerArgs(Args, nbArgs, args...);
};

/**
 * @brief Configures the arguments for each buffer block of the function identified by @p idf.
 * @param idf Index of the registered function.
 * @param shape Array containing the bounds of each created buffer block.
 * @param funcArgs CSPARGS object containing the arguments of the function.
 */
void setArgs(size_t idf, BUFFER_SHAPE shape, CSPARGS funcArgs);
/**
 * @brief Configures the arguments of each regular (same size) buffer block for the function identified by @p idf.
 * @param idf Index of the function.
 * @param workSize Total buffer size (number of elements) used to build regular blocks.
 * @param funcArgs CSPARGS object containing the arguments of the function.
 */
void setArgsRegular(size_t idf, size_t workSize, CSPARGS funcArgs);
/**
 * @brief Reshapes the buffer partitioning by assigning a new shape.
 * @param idf Index of the function.
 * @param shape Array containing the bounds of each created buffer block.
 */
void setBufferShape(size_t idf, BUFFER_SHAPE shape);
/**
 * @brief Reshapes the buffer using regular (same size) blocks, following the new @p workSize parameter.
 * @param idf Index of the function.
 * @param workSize New total buffer size.
 */
void setBufferShapeRegular(size_t idf, size_t workSize);
/**
 * @brief Sets a time delay (in nanoseconds) inside loops to improve safe execution.
 * @param idf Index of the function.
 * @param delay Delay to assign to every thread.
 */
void setDelay(size_t idf, size_t delay);
/**
 * @brief Sets a per-thread time delay (in nanoseconds) for safer execution.
 * @param idf Index of the function.
 * @param delayList List of delay values for each thread.
 */
void setDelay(size_t idf, vector<size_t> delayList);
/**
 * @brief Defines whether the function will be executed normally or in background.
 * @param idf Index of the function.
 * @param execMode Execution mode. Can be CSTHREAD_NORMAL_EXECUTION or CSTHREAD_BACKGROUND_EXECUTION.
 */
void setExecutionMode(size_t idf, bool execMode);
/**
 * @brief Defines whether each thread will be executed normally or in background.
 * @param idf Index of the function.
 * @param execMode List of execution modes for each thread. Each one can be CSTHREAD_NORMAL_EXECUTION or CSTHREAD_BACKGROUND_EXECUTION.
 */
void setExecutionMode(size_t idf, vector<bool> execMode);
/**
 * @brief Creates @p nBlocks buffer blocks of equal size; each block is processed by one thread.
 * @param workSize Total buffer size.
 * @param nBlocks Number of buffer blocks. Each block is managed by one thread. @p nBlocks is reduced to the number of hardware threads if its value is greater.
 * @return BUFFER_SHAPE Array containing all the created buffer blocks.
 */
BUFFER_SHAPE  makeRegularBufferShape(size_t workSize, size_t nBlocks);
/**
 * @brief Registers a new function to be executed in parallel.
 * @param nBlocks Number of buffer blocks, each corresponding to one thread.
 * @param workSize Total buffer size.
 * @param shape Array containing the bounds of each block to be created.
 * @param charfName Name of the function to register.
 * @param blockFunc Pointer to the function to register.
 * @param funcArgs CSPARGS object containing the arguments of the function.
 * @return Index of the registered function.
 */
size_t registerFunction(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, const char* fName, void(*blockFunc)(CSPARGS), CSPARGS funcArgs);
/**
 * @brief Extended registration of a new function. Allows specifying argument pointers directly instead of providing a CSPARGS object, unlike registerFunction.
 * @param nBlocks Number of buffer blocks, each corresponding to a thread.
 * @param workSize Total buffer size.
 * @param shape Array containing the bounds of each block to be created.
 * @param charfName Name of the function to register.
 * @param blockFunc Pointer to the function to register.
 * @param nbArgs Number of arguments passed variadically after @p nbArgs.
 * @return Index of the registered function.
 */
size_t registerFunctionEx(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, const char* fName, void(*blockFunc)(CSPARGS), size_t nbArgs,...);
/**
 * @brief Extended registration of a new function with regular buffer blocks. Arguments are specified directly via pointers.
 * @param nBlocks Number of buffer blocks, each corresponding to a thread.
 * @param workSize Total buffer size used to build regular blocks.
 * @param charfName Name of the function to register.
 * @param blockFunc Pointer to the function to register.
 * @param nbArgs Number of arguments passed variadically after @p nbArgs.
 * @return Index of the registered function.
 */
size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, const char* fName, void(*blockFunc)(CSPARGS), size_t nbArgs,...);
/**
 * @brief Convenience template for extended registration with regular blocks. Builds the argument array from variadic pointers and delegates to registerFunction.
 * @param nBlocks Number of buffer blocks, each corresponding to a thread.
 * @param workSize Total buffer size used to build regular blocks.
 * @param charfName Name of the function to register.
 * @param blockFunc Pointer to the function to register.
 * @param arg Void pointer to the first argument.
 * @param args Other argument pointers (variadic).
 */
template<typename... _Args> size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, const char* fName, void(*Function)(CSPARGS), void*arg, _Args... args)
{
  void **Args = 0;
  size_t nbArgs = 0;

  registerArgs(Args, nbArgs, arg, args...);

  BUFFER_SHAPE shape = makeRegularBufferShape(workSize, nBlocks);
  CSPARGS funcArgs(nbArgs);
  funcArgs.regArgs2(Args,nbArgs);

  size_t idf = registerFunction(nBlocks, workSize, shape, fName, Function, funcArgs);
  free(shape);
  return idf;
};
/**
 * @brief Unregisters the function indexed by @p idf and removes its arguments.
 * @param idf Index of the function to unregister.
 */
void unregisterFunction(size_t idf);
/**
 * @brief Unregisters all registered functions and clears their arguments.
 */
void unregisterAll();
/**
 * @brief Returns min(nThread, getHardwareConcurrency()), where getHardwareConcurrency() gives the number of hardware threads of the CPU.
 * @param nThread Requested number of threads.
 * @return min(nThread, getHardwareConcurrency()).
 */
size_t getSafeThreadNumber(size_t nThread);
/**
 * @brief Returns the number of hardware threads (CPU cores/threads) available on the machine.
 * @return Number of hardware threads.
 */
size_t getHardwareConcurrency();
/**
 * @brief Returns the CSPARGS objects for all buffer blocks of the specified function.
 * @param idf Index of the function.
 * @return Vector of CSPARGS objects, one per buffer block.
 */
vector<CSPARGS> getArgs(size_t idf);
/**
 * @brief Returns the arguments object of the buffer block @p ida for the function @p idf.
 * @param idf Index of the function.
 * @param ida Index of the buffer block.
 * @return CSPARGS arguments object of the specified block.
 */
CSPARGS getArgs(size_t idf, size_t ida);
/**
 * @brief Returns the index of the function identified by its name @p funcName.
 * @return Index of the specified function.
 */
size_t getId(const char*funcName);
/**
 * @brief Returns the index of the function identified by its function pointer @p f.
 * @param f Address of the function.
 * @return Index of the specified function.
 */
size_t getId(void(*f)(CSPARGS));
/**
 * @brief Returns the global buffer size for the function indexed by @p idf.
 * @param idf Index of the function.
 * @return Global buffer size.
 */
size_t getWorkSize(int idf);
/**
 * @brief Updates the argument at index @p ida of function @p idf with @p arg.
 * @param idf Index of the function.
 * @param ida Index of the argument.
 * @param arg Argument to assign.
 */
void updateArg(size_t idf, size_t ida, void* &&arg);
/**
 * @brief Updates a list of arguments of function @p idf.
 * @param idf Index of the function.
 * @param ida List of argument indexes.
 * @param arg List of argument pointers to assign.
 */
void updateArg(size_t idf, initializer_list<size_t> ida, initializer_list<void*> arg);
/**
 * @brief Template overload to update multiple arguments of function @p idf.
 * @param idf Index of the function.
 * @param ida List (array) of argument indexes.
 * @param arg List (array) of argument pointers to assign.
 */
template<size_t N> void updateArg(size_t idf, const size_t (&ida)[N], void* const (&arg)[N]);
/**
 * @brief Runs all buffer blocks of the function @p id in parallel.
 * @param id Index of the function to execute.
 */
void execute(int id);
/**
 * @brief Runs all buffer blocks of the function identified by its name.
 * @param charfuncName Name of the function to execute.
 */
void execute(const char*funcName);
/**
 * @brief Runs all buffer blocks in parallel for the function specified by pointer @p f.
 * @param f Pointer to the function to execute.
 */
void execute(void(*f)(CSPARGS));
/**
 * @brief Joins all threads contained in the given vector.
 * @param threads Vector of threads to execute and join.
 */
void execute(vector<thread> threads);


}

using csFunctionType = void(*)(int);



#endif
