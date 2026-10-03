# CSigma — Parallel Task Module Documentation
**Version 1.0 — Copyright Phil467, 2025**

This document provides a clean API-style reference for the `csParallelTask` component of the CSigma project.
It preserves original descriptions and parameter details but presents them in a modern, readable Markdown layout for GitHub.

---

## Table of Contents
- [csParallel.h](#csparallelh)
  - [Templates & Utilities](#templates--utilities)
  - [Functions](#functions)
- [CSPARGS.h](#cspargsh)
  - [Class `CSPARGS` — Methods & Operators](#class-cspargs---methods--operators)
- [csPerfChecker.h](#csperfcheckerh)
  - [Class `CSPERF_CHECKER` — Methods](#class-csperf_checker---methods)
- [Examples](#examples)

---

## csParallel.h

**Namespace:** `csParallelTask` — Utilities to register functions, prepare argument buffers and execute tasks in parallel.

### Templates & Utilities

#### `template<typename... _Args> void registerArgs(void**& Args, size_t& nbArgs, void* arg, _Args... args)`
```cpp
template<typename... _Args> void registerArgs(void**& Args, size_t& nbArgs, void* arg, _Args... args);
```
**Description**  
Register arguments for some function (at least one argument).

**Parameters**
- **Args** — Returned table that will contain registered arguments (pointer to void* array).  
- **nbArgs** — Number of arguments (will be incremented).  
- **arg** — First argument.  
- **args** — Remaining arguments (variadic).

---

#### `inline void registerArgs(void**& Args, size_t& nbArgs)`
```cpp
inline void registerArgs(void**& Args, size_t& nbArgs);
```
**Description**  
Termination overload for the variadic `registerArgs` template (no-op).

---

### Functions

#### `void setArgs(size_t idf, BUFFER_SHAPE shape, CSPARGS funcArgs)`
```cpp
void setArgs(size_t idf, BUFFER_SHAPE shape, CSPARGS funcArgs);
```
**Description**  
Configures the arguments for each buffer block of the function identified by `idf`.

**Parameters**
- **idf** — Index of the function.  
- **shape** — Table containing the bounds of each created block.  
- **funcArgs** — Object of `CSPARGS` class containing arguments of the function.

---

#### `void setArgsRegular(size_t idf, size_t workSize, CSPARGS funcArgs)`
```cpp
void setArgsRegular(size_t idf, size_t workSize, CSPARGS funcArgs);
```
**Description**  
Configures the arguments of each regular (same size) buffer block for the function identified by `idf`.

**Parameters**
- **idf** — Index of the function.  
- **workSize** — The size of the work, used to build regular blocks.  
- **funcArgs** — Object of `CSPARGS` class containing arguments of the function.

---

#### `void setBufferShape(size_t idf, BUFFER_SHAPE shape)`
```cpp
void setBufferShape(size_t idf, BUFFER_SHAPE shape);
```
**Description**  
Reshapes the buffer partitioning by assigning a new shape.

**Parameters**
- **idf** — Index of the function.  
- **shape** — Table containing the bounds of each created block.

---

#### `void setBufferShapeRegular(size_t idf, size_t workSize)`
```cpp
void setBufferShapeRegular(size_t idf, size_t workSize);
```
**Description**  
Reshapes the buffer using regular (same size) blocks following the new `workSize` parameter.

**Parameters**
- **idf** — Index of the function.  
- **workSize** — Size of the work.

---

#### `void setDelay(size_t idf, size_t delay)`
```cpp
void setDelay(size_t idf, size_t delay);
```
**Description**  
Sets a time delay (in nanoseconds) inside loops to improve safe execution.

**Parameters**
- **idf** — Index of the function.  
- **delay** — Delay to assign to every thread.

---

#### `void setDelay(size_t idf, vector<size_t> delayList)`
```cpp
void setDelay(size_t idf, vector<size_t> delayList);
```
**Description**  
Sets a per-thread time delay (in nanoseconds) using a list of delay values.

**Parameters**
- **idf** — Index of the function.  
- **delayList** — List of delay values for each thread.

---

#### `void setExecutionMode(size_t idf, bool execMode)`
```cpp
void setExecutionMode(size_t idf, bool execMode);
```
**Description**  
Defines whether the function will be executed normally or in background.

**Parameters**
- **idf** — Index of the function.  
- **execMode** — Execution mode. Can be: `CSTHREAD_NORMAL_EXECUTION` or `CSTHREAD_BACKGROUND_EXECUTION`.

---

#### `void setExecutionMode(size_t idf, vector<bool> execMode)`
```cpp
void setExecutionMode(size_t idf, vector<bool> execMode);
```
**Description**  
Defines whether each thread will be executed normally or in background.

**Parameters**
- **idf** — Index of the function.  
- **execMode** — List of execution modes for each thread (`CSTHREAD_NORMAL_EXECUTION` or `CSTHREAD_BACKGROUND_EXECUTION`).

---

#### `BUFFER_SHAPE makeRegularBufferShape(size_t workSize, size_t& nBlocks)`
```cpp
BUFFER_SHAPE makeRegularBufferShape(size_t workSize, size_t& nBlocks);
```
**Description**  
Creates `nBlocks` buffer blocks of equal size; each block is processed by one thread. `nBlocks` is adjusted to CPU hardware threads if necessary.

**Parameters**
- **workSize** — Total buffer size.  
- **nBlocks** — Number of buffer blocks (may be modified).

**Returns**  
Table containing all the created blocks (buffer shape).

---

#### `size_t registerFunction(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char* fName, void(*blockFunc)(CSPARGS), CSPARGS funcArgs)`
```cpp
size_t registerFunction(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char* fName, void(*blockFunc)(CSPARGS), CSPARGS funcArgs);
```
**Description**  
Registers a new function to be executed in parallel.

**Parameters**
- **nBlocks** — Number of buffer blocks, each corresponding to one thread.  
- **workSize** — Total buffer size.  
- **shape** — Array containing the bounds of each block to be created.  
- **fName** — Name of the function to register.  
- **blockFunc** — Pointer to the function to register.  
- **funcArgs** — `CSPARGS` object containing arguments of the function.

**Returns**  
Index of the registered function.

---

#### `size_t registerFunctionEx(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char*fName, void(*blockFunc)(CSPARGS), size_t nbArgs,...)`
```cpp
size_t registerFunctionEx(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char*fName, void(*blockFunc)(CSPARGS), size_t nbArgs,...);
```
**Description**  
Extended registration allowing direct argument pointers (variadic) instead of a `CSPARGS` object.

**Parameters**
- **nBlocks** — Number of buffer blocks.  
- **workSize** — Total buffer size.  
- **shape** — Array containing the bounds of each block.  
- **fName** — Name of the function to register.  
- **blockFunc** — Pointer to the function to register.  
- **nbArgs** — Number of variadic arguments followed by their pointers.

**Returns**  
Index of the registered function.

---

#### `size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, char*fName, void(*blockFunc)(CSPARGS), size_t nbArgs,...)`
```cpp
size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, char*fName, void(*blockFunc)(CSPARGS), size_t nbArgs,...);
```
**Description**  
Extended registration for regular buffer blocks (work divided evenly), allowing variadic argument pointers.

**Parameters**
- **nBlocks** — Number of buffer blocks.  
- **workSize** — Total buffer size used to build regular blocks.  
- **fName** — Name of the function to register.  
- **blockFunc** — Pointer to the function.  
- **nbArgs** — Number of arguments followed by their pointers.

**Returns**  
Index of the registered function.

---

#### `template<typename... _Args> size_t registerFunctionRegularEx(...)` (variadic convenience overload)
```cpp
template<typename... _Args> size_t registerFunctionRegularEx(size_t nBlocks, size_t workSize, char*fName, void(*Function)(CSPARGS), void*arg, _Args... args);
```
**Description**  
Template helper that builds the `Args` array from variadic pointers, constructs a regular buffer shape, creates a `CSPARGS` and calls `registerFunction` internally.

**Parameters**
- **nBlocks** — Number of work blocks.  
- **workSize** — Work size.  
- **fName** — Function name.  
- **Function** — Pointer to the function.  
- **arg, args...** — Variadic list of argument pointers.

**Returns**  
Index of the registered function.

---

#### `void unregisterFunction(size_t idf)`
```cpp
void unregisterFunction(size_t idf);
```
**Description**  
Unregisters the function indexed by `idf` and removes its arguments. Calls `clear()` on all associated `CSPARGS` to avoid memory leaks.

**Parameters**
- **idf** — Index of the function to unregister.

---

#### `void unregisterAll()`
```cpp
void unregisterAll();
```
**Description**  
Unregisters all registered functions and clears their arguments.

---

#### `size_t getSafeThreadNumber(size_t nThread)`
```cpp
size_t getSafeThreadNumber(size_t nThread);
```
**Description**  
Returns `min(nThread, getHardwareConcurrency())`.

**Parameters**
- **nThread** — Number of threads requested.

**Returns**  
Adjusted number of threads to use.

---

#### `size_t getHardwareConcurrency()`
```cpp
size_t getHardwareConcurrency();
```
**Description**  
Gives the number of physical units of the CPU (available threads).

**Returns**  
Number of physical CPU units.

---

#### `vector<CSPARGS> getArgs(size_t idf)`
```cpp
vector<CSPARGS> getArgs(size_t idf);
```
**Description**  
Returns a table that contains the `CSPARGS` arguments objects of all blocks for a function.

**Parameters**
- **idf** — Index of the function.

**Returns**  
Vector of `CSPARGS` objects, one per block.

---

#### `CSPARGS getArgs(size_t idf, size_t ida)`
```cpp
CSPARGS getArgs(size_t idf, size_t ida);
```
**Description**  
Returns the arguments object of the `ida` block for the `idf` function.

**Parameters**
- **idf** — Index of the function.  
- **ida** — Index of the block.

**Returns**  
`CSPARGS` for the specified block.

---

#### `size_t getId(const char* funcName)`
```cpp
size_t getId(const char*funcName);
```
**Description**  
Returns the index of the function identified by its name.

**Parameters**
- **funcName** — Name of the function.

**Returns**  
Index of the specified function.

---

#### `size_t getId(void(*f)(CSPARGS))`
```cpp
size_t getId(void(*f)(CSPARGS));
```
**Description**  
Returns the index of the function identified by its function pointer.

**Parameters**
- **f** — Function pointer.

**Returns**  
Index of the specified function.

---

#### `size_t getWorkSize(int idf)`
```cpp
size_t getWorkSize(int idf);
```
**Description**  
Returns the global buffer size for the function indexed by `idf`.

**Parameters**
- **idf** — Index of the function.

**Returns**  
Global work size.

---

#### `void updateArg(size_t idf, size_t ida, void* &&arg)`
```cpp
void updateArg(size_t idf, size_t ida, void* &&arg);
```
**Description**  
Update the `ida` argument of `idf` function with `arg`.

**Parameters**
- **idf** — Index of the function.  
- **ida** — Index of the argument.  
- **arg** — Argument to assign.

---

#### `void updateArg(size_t idf, initializer_list<size_t> ida, initializer_list<void*> arg)`
```cpp
void updateArg(size_t idf, initializer_list<size_t> ida, initializer_list<void*> arg);
```
**Description**  
Update a list of argument indexes for a function using initializer lists.

**Parameters**
- **idf** — Index of the function.  
- **ida** — List of argument indexes.  
- **arg** — List of argument pointers to assign.

---

#### `template<size_t N> void updateArg(size_t idf, const size_t (&ida)[N], void* const (&arg)[N])`
```cpp
template<size_t N> void updateArg(size_t idf, const size_t (&ida)[N], void* const (&arg)[N]);
```
**Description**  
Template overload to update multiple arguments at once using C-style arrays.

**Parameters**
- **idf** — Index of the function.  
- **ida** — Array of argument indexes.  
- **arg** — Array of argument pointers.

---

#### `void execute(int id)`
```cpp
void execute(int id);
```
**Description**  
Computes every block in parallel for the function identified by `id`.

**Parameters**
- **id** — Index of the function to execute.

---

#### `void execute(const char* funcName)`
```cpp
void execute(const char*funcName);
```
**Description**  
Computes every block in parallel for the function specified by name.

**Parameters**
- **funcName** — Name of the function to execute.

---

#### `void execute(void(*f)(CSPARGS))`
```cpp
void execute(void(*f)(CSPARGS));
```
**Description**  
Computes every block in parallel for the function specified by pointer `f`.

**Parameters**
- **f** — Pointer to the function to execute.

---

#### `void execute(vector<thread> threads)`
```cpp
void execute(vector<thread> threads);
```
**Description**  
Executes a provided vector of `std::thread` objects (utility overload).

**Parameters**
- **threads** — Table of threads to execute.

---

---

## CSPARGS.h

**Class:** `CSPARGS` — Manages arguments and block metadata passed to parallelized functions.

### Constructors & utility

#### `CSPARGS(size_t nArgs=0)`
```cpp
CSPARGS(size_t nArgs=0);
```
**Description**  
Constructs a `CSPARGS` object with optional initial number of arguments.

---

### Methods & Operators

#### `void init(size_t _nbArgs)`
```cpp
void init(size_t _nbArgs);
```
**Description**  
Initializes the object with the specified number of arguments.

**Parameters**
- **_nbArgs** — Number of arguments.

---

#### `void setArg(size_t i, void* arg)`
```cpp
void setArg(size_t i, void* arg);
```
**Description**  
Sets the argument at index `i` to `arg`.

**Parameters**
- **i** — Index of the argument.  
- **arg** — Void pointer to the argument value.

---

#### `void setArgNumber(size_t nbArgs)`
```cpp
void setArgNumber(size_t nbArgs);
```
**Description**  
Sets up the number of arguments.

**Parameters**
- **nbArgs** — Number of arguments.

---

#### `void setBounds(CSPARGS::BOUNDS bounds)`
```cpp
void setBounds(CSPARGS::BOUNDS bounds);
```
**Description**  
Sets the work block bounds.

**Parameters**
- **bounds** — `BOUNDS` structure with `first` and `last` indices.

---

#### `void setBlockId(size_t id)`
```cpp
void setBlockId(size_t id);
```
**Description**  
Saves the current block index.

**Parameters**
- **id** — Index of the block.

---

#### `void setBlocksNumber(size_t blockNumber)`
```cpp
void setBlocksNumber(size_t blockNumber);
```
**Description**  
Saves the number of work blocks created.

**Parameters**
- **blockNumber** — Number of work blocks created.

---

#### `void setWorkSize(size_t workSize)`
```cpp
void setWorkSize(size_t workSize);
```
**Description**  
Sets the work size.

**Parameters**
- **workSize** — Work size.

---

#### `void setDelay(size_t delay)`
```cpp
void setDelay(size_t delay);
```
**Description**  
Sets time delay for the thread.

**Parameters**
- **delay** — Time delay in (expected nanoseconds).

---

#### Sleep helpers
```cpp
void sleepHour();
void sleepMin();
void sleepSec();
void sleepMilli();
void sleepMicro();
void sleepNano();
```
**Description**  
Convenience methods to sleep the current thread by hours, minutes, seconds, milliseconds, microseconds, or nanoseconds.

---

#### `void* getArg(size_t i)`
```cpp
void* getArg(size_t i);
```
**Description**  
Returns the argument at index `i`.

**Parameters**
- **i** — Argument index.

**Returns**  
Void pointer to the argument.

---

#### `size_t getBlockId()`
```cpp
size_t getBlockId();
```
**Description**  
Returns the current block index.

**Returns**  
Current block index.

---

#### `size_t getBlocksNumber()`
```cpp
size_t getBlocksNumber();
```
**Description**  
Returns the number of work blocks created.

**Returns**  
Number of blocks.

---

#### `CSPARGS::BOUNDS getBounds()`
```cpp
CSPARGS::BOUNDS getBounds();
```
**Description**  
Returns the work block bounds for this `CSPARGS` instance.

**Returns**  
`BOUNDS` structure with `first` and `last`.

---

#### `void getBounds(size_t workSize, size_t* min, size_t* max)`
```cpp
void getBounds(size_t workSize, size_t*min, size_t*max);
```
**Description**  
Calculates regular bounds of the work block following `workSize` and returns them via `min` and `max` pointers.

**Parameters**
- **workSize** — Global size.  
- **min** — Output lower bound.  
- **max** — Output upper bound.

---

#### `size_t getArgNumber()`
```cpp
size_t getArgNumber();
```
**Description**  
Returns the number of registered arguments.

**Returns**  
Number of arguments.

---

#### `template <class T> T* getArgPtr(size_t idArg)`
```cpp
template <class T> T* getArgPtr(size_t idArg);
```
**Description**  
Returns a typed pointer to the argument at `idArg` (internal storage offset +2).

**Parameters**
- **idArg** — Index of the argument.

**Returns**  
Pointer of type `T*` to the stored argument.

---

#### `size_t getWorkSize()`
```cpp
size_t getWorkSize();
```
**Description**  
Returns the stored work size.

**Returns**  
Work size value.

---

#### `void regArgs(...)` and `void regArgs2(void** args, size_t nbArgs)`
```cpp
void regArgs(...);
void regArgs2(void** args, size_t nbArgs);
template<size_t _nbArgs> void regArgs2(void* args[_nbArgs]);
```
**Description**  
Initialize the internal argument array from variadic arguments or from a provided array of void pointers.

**Parameters for regArgs2(void** args, size_t nbArgs)**
- **args** — Table of void pointers to argument addresses.  
- **nbArgs** — Number of arguments.

---

#### `void lockGuard()`
```cpp
void lockGuard();
```
**Description**  
Activates an internal mutex for safe coordination when accessing shared resources.

---

#### `void clear()`
```cpp
void clear();
```
**Description**  
Releases internal resources of the `CSPARGS` object.

---

#### Conversion & operator overloads
```cpp
operator CSPARGS::BOUNDS();
operator size_t();
void* operator[](size_t i);
void operator=(csVOID_ARG va);
```
**Description**  
- `operator CSPARGS::BOUNDS()` — returns bounds.  
- `operator size_t()` — returns the block index.  
- `operator[]` — returns argument pointer at index.  
- `operator=` — assign a `csVOID_ARG` to set argument.

---

#### `bool EXEC_MODE`
```cpp
bool EXEC_MODE = CSTHREAD_NORMAL_EXECUTION;
```
**Description**  
Defines whether the thread will be executed normally or in background. Can be `CSTHREAD_NORMAL_EXECUTION` or `CSTHREAD_BACKGROUND_EXECUTION`.

---

## csPerfChecker.h

**Class:** `CSPERF_CHECKER` — Simple timing utility for measuring code execution.

### Constants
```cpp
#define CSTIME_UNIT_HOUR            0
#define CSTIME_UNIT_MINUTE          1
#define CSTIME_UNIT_SECOND          2
#define CSTIME_UNIT_MILLISECOND     3
#define CSTIME_UNIT_MICROSECOND     4
#define CSTIME_UNIT_NANOSECOND      5
```

### Methods

#### `CSPERF_CHECKER(int unit = CSTIME_UNIT_MICROSECOND)`
```cpp
CSPERF_CHECKER(int unit = CSTIME_UNIT_MICROSECOND);
```
**Description**  
Constructor. Optionally sets the time unit for measurements.

**Parameters**
- **unit** — Time unit (use defined constants).

---

#### `void setTimeUnit(int unit)`
```cpp
void setTimeUnit(int unit);
```
**Description**  
Sets up the time unit for reported measurements.

**Parameters**
- **unit** — Time unit constant.

---

#### `void start()`
```cpp
void start();
```
**Description**  
Starts measuring execution time.

---

#### `void stop()`
```cpp
void stop();
```
**Description**  
Stops the measurement and records elapsed time.

---

#### `void printReport(const char* title)`
```cpp
void printReport(const char* title);
```
**Description**  
Prints the execution time for a code block with a given title.

**Parameters**
- **title** — Output title.

---

#### `size_t getEllapsedTime()`
```cpp
size_t getEllapsedTime();
```
**Description**  
Returns the last measured execution time.

**Returns**  
Elapsed time in the configured unit.

---

## Examples

### Example 1 — Parallel computation with `csParallelTask`
```cpp
void compute(CSPARGS args) {
    auto b = args.getBounds();
    double* data = args.getArgPtr<double>(0);
    for(size_t i = b.first; i < b.last; ++i)
        data[i] = sqrt(data[i]);
}

int main() {
    size_t workSize = 100000;
    size_t nBlocks = 8;
    double data[workSize];

    size_t id = csParallelTask::registerFunctionRegularEx(
        nBlocks, workSize, "sqrtTask", compute, (void*)data
    );
    csParallelTask::execute(id);
}
```

### Example 2 — Using `CSPARGS` to handle thread-specific data
```cpp
CSPARGS args(3);
int x = 1, y = 2, z = 3;
args.regArgs2((void*[]){&x, &y, &z});
args.setBounds({0, 100});
args.setDelay(1000);
```

### Example 3 — Measuring performance
```cpp
CSPERF_CHECKER perf(CSTIME_UNIT_MICROSECOND);
perf.start();
// Code block to measure
perf.stop();
perf.printReport("Parallel execution time");
```

---
_End of documentation._
