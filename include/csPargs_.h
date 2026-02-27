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

class CS_PARALLEL_TASK_API CSPARGS
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

    CSPARGS(size_t nArgs=0);
/**
 * @brief Initializes the object.
 * @param _nbArgs Number of arguments.
 */
    void init(size_t _nbArgs);
/**
 * @brief Sets the argument at index @p i.
 * @param i Index of the argument.
 * @param arg Void pointer containing the argument value to assign.
 */
    void  setArg(size_t i, void* arg);
/**
 * @brief Sets the number of arguments.
 * @param nbArgs Number of arguments.
 */
    void setArgNumber(size_t nbArgs);
/**
 * @brief Sets the buffer block bounds.
 * @param bounds Structure CSPARGS::BOUNDS that contains the bounds of the buffer block.
 */
    void setBounds(CSPARGS::BOUNDS bounds);
/**
 * @brief Saves the current block index.
 * @param id Index of the block.
 */
    void setBlockId(size_t id);
/**
 * @brief Saves the number of buffer blocks created.
 * @param blockNumber Number of buffer blocks created.
 */
    void setBlocksNumber(size_t blockNumber);
/**
 * @brief Sets the buffer size.
 * @param workSize Buffer size.
 */
    void setWorkSize(size_t workSize);
/**
 * @brief Sets a time delay (in nanoseconds) in the loop for safer thread execution.
 * @param delay Time delay.
 */
    void setDelay(size_t delay);
/**
 * @brief Applies an hours delay in the thread.
 */
    void sleepHour();
/**
 * @brief Applies a minutes delay in the thread.
 */
    void sleepMin();
/**
 * @brief Applies a seconds delay in the thread.
 */
    void sleepSec();
/**
 * @brief Applies a milliseconds delay in the thread.
 */
    void sleepMilli();
/**
 * @brief Applies a microseconds delay in the thread.
 */
    void sleepMicro();
/**
 * @brief Applies a nanoseconds delay in the thread.
 */
    void sleepNano();
/**
/**
 * @brief Returns the argument indexed by @p i.
 * @param i Index of the argument.
 * @return Void pointer containing the argument.
 */
    void* getArg(size_t i);
/**
 * @brief Returns the current block index.
 * @return Current block index.
 */
    size_t getBlockId();
/**
 * @brief Returns the number of buffer blocks created.
 * @return Number of buffer blocks created.
 */
    size_t getBlocksNumber();
/**
 * @brief Returns the buffer block bounds.
 * @return CSPARGS::BOUNDS structure that contains the bounds of the buffer block.
 */
    CSPARGS::BOUNDS getBounds();
/**
 * @brief Calculates regular bounds of a buffer block from the given @p workSize.
 * @param workSize Global buffer size.
 * @param min Output lower bound.
 * @param max Output upper bound.
 */
    void getBounds(size_t workSize, size_t*min, size_t*max);

/**
 * @brief Returns the number of registered arguments.
 * @return Number of registered arguments.
 */
    size_t getArgNumber();
/**
 * @brief Returns a typed pointer to the argument indexed by @p idArg.
 * @param idArg Index of the argument.
 * @return T* Pointer to the argument.
 */
    template <class T> T* getArgPtr(size_t idArg)
    {
        return (T*)Args[idArg+2];
    };
/**
 * @brief Returns a typed pointer to the argument indexed by @p idArg.
 * @param idArg Index of the argument.
 * @return T* Pointer to the argument.
 */
    template <class T> T* getArgPtr(int idArg)
    {
        return (T*)Args[idArg+2];
    };
/**
 * @brief Returns the buffer size.
 * @return Buffer size.
 */
    size_t getWorkSize();
/**
 * @brief Sets up arguments from a variadic list.
 * @param ... Argument pointers.
 */
    void regArgs(void*arg,...);
/**
 * @brief Sets up arguments from a table of pointers.
 * @param args Table of void pointers to argument addresses.
 * @param nbArgs Number of arguments.
 */
    void  regArgs2(void** args, size_t nbArgs);
/**
 * @brief Sets up arguments from a fixed-size table of pointers.
 * @param args Table of void pointers to argument addresses.
 * @param _nbArgs Number of arguments.
 */
    template<size_t _nbArgs> void regArgs2(void*args[_nbArgs]);
/**
 * @brief Activates a mutex for safe thread coordination when accessing the same resources.
 */
    void lockGuard();
/**
 * @brief Releases internal resources associated with this object.
 */
    void  clear();
/**
 * @brief Returns the bounds of the buffer section where the current thread executes.
 * @return CSPARGS::BOUNDS Bounds of the buffer section.
 */
    operator CSPARGS::BOUNDS();
/**
 * @brief Returns the index of the current thread (block id).
 * @return Index of the current thread.
 */
    operator size_t();
/**
 * @brief Returns a void pointer to the argument indexed by @p i.
 * @return Void pointer to the argument.
 */
    void* operator[](size_t i);
/**
 * @brief Assigns a new void pointer to the argument indexed by the member @p i of the @p va object.
 */
    void operator=(csVOID_ARG va);
/**
 * @brief Defines whether the thread will be executed normally or in background. Can be CSTHREAD_NORMAL_EXECUTION or CSTHREAD_BACKGROUND_EXECUTION.
 */
    bool EXEC_MODE = CSTHREAD_NORMAL_EXECUTION;


private:
    void** Args;
    size_t nbArgs;
    size_t blocksNumber;
    size_t blockId;
    BOUNDS bounds;
    size_t workSize;
    size_t delay;
};

#endif
