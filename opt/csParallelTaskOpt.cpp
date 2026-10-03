#include "csParallelTaskOpt.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <functional>

#ifdef CS_STATIC_LIB
#undef CS_FORCE_INLINE
#undef CS_INLINE
#define CS_FORCE_INLINE __attribute__((used))
#define CS_INLINE __attribute__((used))
#endif


/* ---- src/csPargs.cpp ---- */
std::mutex _mutex;

CS_FORCE_INLINE CSPARGS::CSPARGS(size_t _nbArgs)
{
    init(_nbArgs);
};

CS_FORCE_INLINE void CSPARGS::init(size_t _nbArgs)
{
    Args = 0;
    nbArgs = _nbArgs;
    bounds = {0,0};
    blockId = 0;
    workSize = 0;
    Args = (void**)malloc(sizeof(void*)*(nbArgs+2));
    Args[1] = (void*)&bounds;
    Args[0] = (void*)&blockId;
}

CS_FORCE_INLINE void* CSPARGS::getArg(size_t i)
{
    return Args[i+2];
}

CS_FORCE_INLINE CSPARGS::BOUNDS CSPARGS::getBounds()
{
    return bounds;
}

CS_FORCE_INLINE void CSPARGS::setBounds(CSPARGS::BOUNDS b)
{
    bounds = b;

}

CS_FORCE_INLINE void CSPARGS::setWorkSize(size_t _workSize)
{
    workSize = _workSize;

}

CS_FORCE_INLINE size_t CSPARGS::getWorkSize()
{
    return workSize;
}

CS_FORCE_INLINE size_t CSPARGS::getBlockId()
{
    return blockId;
}

CS_FORCE_INLINE size_t CSPARGS::getArgNumber()
{
    return nbArgs;
}

CS_FORCE_INLINE void CSPARGS::setArg(size_t i, void*arg)
{
    Args[i+2] = arg;
}

CS_FORCE_INLINE void CSPARGS::setDelay(size_t _delay)
{
    delay = _delay;
}

CS_FORCE_INLINE void CSPARGS::sleepHour()
{
    std::this_thread::sleep_for(std::chrono::hours(delay));
}

CS_FORCE_INLINE void CSPARGS::sleepMin()
{
    std::this_thread::sleep_for(std::chrono::minutes(delay));
}

CS_FORCE_INLINE void CSPARGS::sleepSec()
{
    std::this_thread::sleep_for(std::chrono::seconds(delay));
}

CS_FORCE_INLINE void CSPARGS::sleepMilli()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

CS_FORCE_INLINE void CSPARGS::sleepMicro()
{
    std::this_thread::sleep_for(std::chrono::microseconds(delay));
}

CS_FORCE_INLINE void CSPARGS::sleepNano()
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
}

CS_FORCE_INLINE void CSPARGS::setBlockId(size_t id)
{
    blockId = id;
}

CS_FORCE_INLINE void CSPARGS::setBlocksNumber(size_t _blocksNumber)
{
  blocksNumber = _blocksNumber;
}

CS_FORCE_INLINE size_t CSPARGS::getBlocksNumber()
{
  return blocksNumber;
}

CS_FORCE_INLINE void CSPARGS::getBounds(size_t workSize, size_t*min, size_t*max)
{
  size_t delta = workSize/blocksNumber;
  *min = blockId*delta;
  if(blockId == blocksNumber-1)
    *max = workSize;
  else
    *max = (blockId + 1)*delta;
}


CS_FORCE_INLINE void CSPARGS::setArgNumber(size_t _nbArgs)
{
    nbArgs = _nbArgs;
    int m = nbArgs + 2;
    Args = (void**)realloc(Args, m*sizeof(void*));
}


CS_INLINE void CSPARGS::regArgs(void*arg,...)
{
    va_list adArgs ;
    void* parv;
    va_start (adArgs, arg);
    Args[2]=arg;
    for (int i=0 ; i<nbArgs ; i++)
    {
        parv = va_arg (adArgs, void*) ;
        Args[i+3] = parv;
    }
    va_end(adArgs);
}

template<size_t _nbArgs> CS_FORCE_INLINE void CSPARGS::regArgs2(void*args[_nbArgs])
{
    for (int i=0 ; i<_nbArgs ; i++)
    {
        Args[i+2] = args[i];
    }
}

CS_FORCE_INLINE void CSPARGS::regArgs2(void**args, size_t nbArgs)
{
    for (int i=0 ; i<nbArgs ; i++)
    {
        Args[i+2] = args[i];
    }
}

CS_FORCE_INLINE void CSPARGS::lockGuard()
{
    std::lock_guard<std::mutex> lock(_mutex);
}

CS_FORCE_INLINE void CSPARGS::clear()
{
    free(Args);
    Args = 0;
    bounds = {0};
    blockId = 0;
    workSize = 0;
}

CS_FORCE_INLINE CSPARGS::operator CSPARGS::BOUNDS()
{
    return bounds;
}

CS_FORCE_INLINE CSPARGS::operator size_t()
{
    return blockId;
};

CS_FORCE_INLINE void* CSPARGS::operator[](size_t i)
{
    return Args[i+2];
};

CS_FORCE_INLINE void CSPARGS::operator=(csVOID_ARG va)
{
    Args[va.i+2] = va.arg;
};

/* ---- src/csPerfChecker.cpp ---- */
CS_FORCE_INLINE CSPERF_CHECKER::CSPERF_CHECKER(int _unit)
{
    setTimeUnit(_unit);
}

CS_FORCE_INLINE void CSPERF_CHECKER::setTimeUnit(int _unit)
{
    unit = _unit;
}

CS_FORCE_INLINE void CSPERF_CHECKER::start()
{
   strt = std::chrono::high_resolution_clock::now();
}

CS_FORCE_INLINE void CSPERF_CHECKER::stop()
{
  stp = std::chrono::high_resolution_clock::now();
  // Calculer la dur�e en microsecondes (ou autre unit�)

  switch (unit)
  {
  case CSTIME_UNIT_HOUR:
    ellapsed = std::chrono::duration_cast<std::chrono::hours>(stp - strt).count();
    unitName = " hours\0";
    break;
  case CSTIME_UNIT_MINUTE:
    ellapsed = std::chrono::duration_cast<std::chrono::minutes>(stp - strt).count();
    unitName = " minutes\0";
    break;
  case CSTIME_UNIT_SECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::seconds>(stp - strt).count();
    unitName = " seconds\0";
    break;
  case CSTIME_UNIT_MILLISECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stp - strt).count();
    unitName = " milliseconds\0";
    break;
  case CSTIME_UNIT_MICROSECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::microseconds>(stp - strt).count();
    unitName = " microseconds\0";
    break;
  case CSTIME_UNIT_NANOSECOND:
    ellapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stp - strt).count();
    unitName = " nanoseconds\0";
    break;

  default:
    ellapsed = std::chrono::duration_cast<std::chrono::microseconds>(stp - strt).count();
    unitName = " microseconds\0";
    break;
  }

}

CS_FORCE_INLINE void CSPERF_CHECKER::printReport(const char* title)
{
  std::cout<< title;
  std::cout << "Execution time : " << ellapsed << unitName << " \n";
}

CS_FORCE_INLINE size_t CSPERF_CHECKER::getEllapsedTime()
{
  return ellapsed;
}

/* ---- src/csParallel.cpp ---- */
using namespace std;

vector<void(*)(CSPARGS)> BLOCK_FUNC;
vector<vector<CSPARGS>> BLOCK_ARGS;
vector<string> THREAD_NAME;
vector<size_t> THREAD_GLOBAL_SIZE;

using namespace csParallelTask;

/****************************************/


CS_FORCE_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::getHardwareConcurrency()
{
    return std::thread::hardware_concurrency();
}

CS_FORCE_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::getSafeThreadNumber(size_t n)
{
  return std::min(n,getHardwareConcurrency());
}

CS_FORCE_INLINE CSPARGS CS_PARALLEL_TASK_API csParallelTask::getArgs(size_t idf, size_t ida)
{
  return BLOCK_ARGS[idf][ida];
}

CS_FORCE_INLINE vector<CSPARGS> CS_PARALLEL_TASK_API csParallelTask::getArgs(size_t idf)
{
  return BLOCK_ARGS[idf];
}

CS_FORCE_INLINE BUFFER_SHAPE CS_PARALLEL_TASK_API csParallelTask::makeRegularBufferShape(size_t workSize, size_t nBlocks)
{
  BUFFER_SHAPE  bp;
  if (nBlocks>0)
  {
    bp = _csAlloc<CSPARGS::BOUNDS>(nBlocks);
    size_t len = workSize/nBlocks;

    for(int i=0; i<nBlocks; i++)
    {
      bp[i] = {i*len, (i+1)*len};
    }
    bp[nBlocks-1].last = workSize;
  }
  else
    cout<<"invalid block size !\n";
  return bp;
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setArgs(size_t idf, BUFFER_SHAPE shape, CSPARGS funcArgs)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  if (nBlocks > 0)
  {
    vector<CSPARGS> *arg = &BLOCK_ARGS[idf];

    size_t nbArgs = funcArgs.getArgNumber();

    for(size_t i=0; i<nBlocks; i++)
    {
      (*arg)[i].clear();
      (*arg)[i].setArgNumber(nbArgs);
      (*arg)[i].setBlockId(i);
      (*arg)[i].setBounds(shape[i]);
      (*arg)[i].setBlocksNumber(nBlocks);

      for(size_t j=0; j<nbArgs; j++)
      {
        (*arg)[i].setArg(j,funcArgs.getArg(j));
      }
    }
  }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setArgsRegular(size_t idf, size_t workSize, CSPARGS funcArgs)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  BUFFER_SHAPE shape = makeRegularBufferShape(workSize, nBlocks);
  setArgs(idf, shape, funcArgs);
  free(shape);
}

CS_FORCE_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::registerFunction(size_t _nBlocks, size_t workSize, BUFFER_SHAPE shape, const char* fName, void(*Function)(CSPARGS), CSPARGS funcArgs)
{
  size_t nBlocks = getSafeThreadNumber(_nBlocks);
  size_t k = BLOCK_FUNC.size();
  if (nBlocks > 0)
  {
    vector<CSPARGS> pargs;
    CSPARGS arg[nBlocks];

    size_t nbArgs = funcArgs.getArgNumber();
    for(size_t i=0; i<nBlocks; i++)
    {
      arg[i].setArgNumber(nbArgs);
      arg[i].setBlockId(i);
      arg[i].setBounds(shape[i]);
      arg[i].setBlocksNumber(nBlocks);
      arg[i].setWorkSize(workSize);
      arg[i].setDelay(1);

      for(size_t j=0; j<nbArgs; j++)
      {
        arg[i].setArg(j,funcArgs.getArg(j));
      }

      pargs.push_back(arg[i]);
    }

    BLOCK_FUNC.push_back(Function);
    BLOCK_ARGS.push_back(pargs);
    THREAD_GLOBAL_SIZE.push_back(workSize);

    if(!(char*)fName)
    {
      char str[100] = {0};
      sprintf(str,"%zu", THREAD_NAME.size());
      THREAD_NAME.push_back(str);
    }
    else
    {
      THREAD_NAME.push_back((char*)fName);
    }
    }
    else
      cout<<"invalid block size !\n";

    return k;
}

CS_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::registerFunctionEx(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, const char* fName, void(*Function)(CSPARGS), size_t nbArgs,...)
{
  va_list adArgs ;
  void* parv;
  va_start (adArgs, nbArgs);
  void *Args[nbArgs];

  for (int i=0 ; i<nbArgs ; i++)
  {
    Args[i] = va_arg (adArgs, void*);
  }
  va_end(adArgs);

  CSPARGS funcArgs(nbArgs);
  funcArgs.regArgs2(Args,nbArgs);
  return registerFunction(nBlocks, workSize, shape, fName, Function, funcArgs);
}

CS_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::registerFunctionRegularEx(size_t nBlocks, size_t workSize, const char* fName, void(*Function)(CSPARGS), size_t nbArgs,...)
{
  va_list adArgs ;
  void* parv;
  va_start(adArgs, nbArgs);
  void *Args[nbArgs];

  for (int i=0 ; i<nbArgs ; i++)
  {
    Args[i] = va_arg (adArgs, void*);
  }
  va_end(adArgs);

  BUFFER_SHAPE shape = makeRegularBufferShape(workSize, nBlocks);
  CSPARGS funcArgs(nbArgs);
  funcArgs.regArgs2(Args,nbArgs);

  size_t idf = registerFunction(nBlocks, workSize, shape, fName, Function, funcArgs);
  free(shape);
  return idf;
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::unregisterFunction(size_t idf)
{
  if (idf >= BLOCK_FUNC.size())
  {
    cout<<"invalid function id !\n";
    return;
  }

  // Libérer correctement les CSPARGS associés
  size_t nBlocks = BLOCK_ARGS[idf].size();
  for(size_t i = 0; i < nBlocks; i++)
  {
    BLOCK_ARGS[idf][i].clear();
  }

  BLOCK_FUNC.erase(BLOCK_FUNC.begin() + idf);
  BLOCK_ARGS.erase(BLOCK_ARGS.begin() + idf);
  THREAD_NAME.erase(THREAD_NAME.begin() + idf);
  THREAD_GLOBAL_SIZE.erase(THREAD_GLOBAL_SIZE.begin() + idf);
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::unregisterAll()
{
  // Libérer tous les CSPARGS avant de vider les vecteurs
  size_t nFuncs = BLOCK_ARGS.size();
  for(size_t f = 0; f < nFuncs; f++)
  {
    size_t nBlocks = BLOCK_ARGS[f].size();
    for(size_t i = 0; i < nBlocks; i++)
    {
      BLOCK_ARGS[f][i].clear();
    }
  }

  BLOCK_FUNC.clear();
  BLOCK_ARGS.clear();
  THREAD_NAME.clear();
  THREAD_GLOBAL_SIZE.clear();
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::execute(int id)
{
  int nBlocks = BLOCK_ARGS[id].size();

  thread v[nBlocks];
  for (size_t n = 0; n < nBlocks; n++)
  {
    v[n] = thread(
      [](int i,int k)
      {
         BLOCK_FUNC[k](BLOCK_ARGS[k][i]);
      },
      n,id);
  }

  for(int i = 0; i<nBlocks; i++)
  {
      if(BLOCK_ARGS[id][i].EXEC_MODE == CSTHREAD_NORMAL_EXECUTION)
        v[i].join();
      else
        v[i].detach();
  }
}


CS_FORCE_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::getId(const char*funcName)
{
  size_t id = 0;
  size_t n = THREAD_NAME.size();
  for(size_t i=0; i<n; i++)
  {
    if(strcmp(THREAD_NAME[i].c_str(),funcName)==0)
    {
      id = i;
      break;
    }
  }
  return id;
}

CS_FORCE_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::getId(void(*f)(CSPARGS))
{
  size_t id = 0;
  size_t n = THREAD_NAME.size();
  for(size_t i=0; i<n; i++)
  {
    if(f == BLOCK_FUNC[i])
    {
      id = i;
      break;
    }
  }
  return id;
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::execute(vector<std::thread> threads)
{
  size_t n = threads.size();
  for(size_t i=0; i<n; i++)
  {
    threads[i].join();
  }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::execute(const char*funcName)
{
  size_t id = getId(funcName);
  execute(id);
}

CS_FORCE_INLINE void csParallelTask::execute(void(*f)(CSPARGS))
{
  size_t id = getId(f);
  execute(id);
}

CS_FORCE_INLINE size_t CS_PARALLEL_TASK_API csParallelTask::getWorkSize(int idf)
{
    return THREAD_GLOBAL_SIZE[idf];
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setBufferShape(size_t idf, BUFFER_SHAPE shape)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<CSPARGS> parg = BLOCK_ARGS[idf];
  for(size_t i=0; i<nBlocks; i++)
  {
    parg[i].setBounds(shape[i]);
  }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setDelay(size_t idf, size_t delay)
{
    size_t nBlocks = BLOCK_ARGS[idf].size();
    for(size_t i=0; i<nBlocks; i++)
    {
        BLOCK_ARGS[idf][i].setDelay(delay);
    }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setDelay(size_t idf, vector<size_t> delayList)
{
    size_t nBlocks = BLOCK_ARGS[idf].size();
    if (nBlocks > delayList.size())
    {
        perror("Incomplete delay list !");
        return;
    }
    for(size_t i=0; i<nBlocks; i++)
    {
        BLOCK_ARGS[idf][i].setDelay(delayList[i]);
    }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setBufferShapeRegular(size_t idf, size_t workSize)
{
  if(csParallelTask::getWorkSize(idf) != workSize)
  {
      size_t nBlocks = BLOCK_ARGS[idf].size();
      BUFFER_SHAPE shape = makeRegularBufferShape(workSize, nBlocks);
      setBufferShape(idf,shape);
      free(shape);
      THREAD_GLOBAL_SIZE[idf] = workSize;
  }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setExecutionMode(size_t idf, bool execMode)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  for(int i=0; i<nBlocks; i++)
  {
      BLOCK_ARGS[idf][i].EXEC_MODE = execMode;
  }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::setExecutionMode(size_t idf, vector<bool> execMode)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  if (nBlocks > execMode.size())
  {
      perror("Incomplete execMode list !");
      return;
  }
  for(int i=0; i<nBlocks; i++)
  {
      BLOCK_ARGS[idf][i].EXEC_MODE = execMode[i];
  }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::updateArg(size_t idf, size_t ida, void*&&arg)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<CSPARGS> parg = BLOCK_ARGS[idf];
  for(size_t i=0; i<nBlocks; i++)
  {
    parg[i].setArg(ida,arg);
  }
}

template<size_t N> CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::updateArg(size_t idf, const size_t (&ida)[N], void* const (&arg)[N])
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<CSPARGS>parg = BLOCK_ARGS[idf];

  for(size_t i=0; i<nBlocks; i++)
  {
    for(size_t j=0; j<N; j++)
      parg[i].setArg(ida[j],arg[j]);
  }
}

CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::updateArg(size_t idf, initializer_list<size_t>ida, initializer_list<void*> arg)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<CSPARGS>parg = BLOCK_ARGS[idf];

  for(size_t i=0; i<nBlocks; i++)
  {
    auto a = arg.begin();
    size_t j = 0;
    for(size_t id : ida)
    {
      advance(a,j);
      parg[i].setArg(id,*a);
      j++;
    }
  }
}

template<class T>CS_FORCE_INLINE void CS_PARALLEL_TASK_API csParallelTask::updateArg(size_t idf, size_t ida, T arg)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<CSPARGS>parg = BLOCK_ARGS[idf];
  for(size_t i=0; i<nBlocks; i++)
  {
    *(T*)parg[i] = arg;
  }
}
