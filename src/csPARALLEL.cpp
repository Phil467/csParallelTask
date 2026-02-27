
#include <iostream>
#include <functional>
#include <thread>
#include "csPargs.h"
#include "csParallel.h"


using namespace std;

vector<void(*)(csPARGS)> BLOCK_FUNC;
vector<vector<csPARGS>> BLOCK_ARGS;
vector<string> THREAD_NAME;
vector<size_t> THREAD_GLOBAL_SIZE;

using namespace csParallelTask;

/****************************************/


size_t CSPARALLEL_API csParallelTask::getMaxThreadNumber()
{
    return std::thread::hardware_concurrency();
}

size_t CSPARALLEL_API csParallelTask::getRightThreadNumber(size_t n)
{
  return std::min(n,getMaxThreadNumber());
}

csPARGS CSPARALLEL_API csParallelTask::getArgs(size_t idf, size_t ida)
{
  return BLOCK_ARGS[idf][ida];
}

vector<csPARGS> CSPARALLEL_API csParallelTask::getArgs(size_t idf)
{
  return BLOCK_ARGS[idf];
}

BUFFER_SHAPE CSPARALLEL_API csParallelTask::makeRegularBufferShape(size_t workSize, size_t& nBlocks)
{
  nBlocks = getRightThreadNumber(nBlocks);
  BUFFER_SHAPE  bp;
  if (nBlocks>0)
  {
    bp = _csAlloc<csPARGS::BOUNDS>(nBlocks);
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

void CSPARALLEL_API csParallelTask::setArgs(size_t idf, BUFFER_SHAPE shape, csPARGS funcArgs)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  if (nBlocks>0)
  {
    vector<csPARGS> *arg = &BLOCK_ARGS[idf];

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

void CSPARALLEL_API csParallelTask::setArgsRegular(size_t idf, size_t workSize, csPARGS funcArgs)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  BUFFER_SHAPE shape = makeRegularBufferShape(workSize, nBlocks);
  setArgs(idf, shape, funcArgs);
  free(shape);
}

size_t CSPARALLEL_API csParallelTask::registerFunction(size_t _nBlocks, size_t workSize, BUFFER_SHAPE shape, char*fName, void(*Function)(csPARGS), csPARGS funcArgs)
{
  size_t nBlocks = getRightThreadNumber(_nBlocks);
  size_t k = BLOCK_FUNC.size();
  if (nBlocks>0)
  {
    vector<csPARGS> pargs;
    csPARGS arg[nBlocks];

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

    if(!fName)
    {
      char str[100];
      sprintf(str,"%zu", THREAD_NAME.size());
      THREAD_NAME.push_back(str);
    }
    else
    {
      THREAD_NAME.push_back(fName);
    }
    }
    else
      cout<<"invalid block size !\n";

    return k;
}

size_t CSPARALLEL_API csParallelTask::registerFunctionEx(size_t nBlocks, size_t workSize, BUFFER_SHAPE shape, char*fName, void(*Function)(csPARGS), size_t nbArgs,...)
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

  csPARGS funcArgs(nbArgs);
  funcArgs.makeArgs2(Args,nbArgs);
  return registerFunction(nBlocks, workSize, shape, fName, Function, funcArgs);

}

size_t CSPARALLEL_API csParallelTask::registerFunctionRegularEx(size_t nBlocks, size_t workSize, char*fName, void(*Function)(csPARGS), size_t nbArgs,...)
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
  csPARGS funcArgs(nbArgs);
  funcArgs.makeArgs2(Args,nbArgs);

  size_t idf = registerFunction(nBlocks, workSize, shape, fName, Function, funcArgs);
  free(shape);
  return idf;
}

void CSPARALLEL_API csParallelTask::unregisterFunction(size_t idf)
{
  if (idf >= BLOCK_FUNC.size())
  {
    cout<<"invalid function id !\n";
    return;
  }

  // Libérer correctement les csPARGS associés
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

void CSPARALLEL_API csParallelTask::unregisterAll()
{
  // Libérer tous les csPARGS avant de vider les vecteurs
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

void CSPARALLEL_API csParallelTask::execute(int id)
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


size_t CSPARALLEL_API csParallelTask::getId(const char*funcName)
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

size_t CSPARALLEL_API csParallelTask::getId(void(*f)(csPARGS))
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

void CSPARALLEL_API csParallelTask::execute(vector<std::thread> threads)
{
  size_t n = threads.size();
  for(size_t i=0; i<n; i++)
  {
    threads[i].join();
  }
}

void CSPARALLEL_API csParallelTask::execute(const char*funcName)
{
  size_t id = getId(funcName);
  execute(id);
}

void csParallelTask::execute(void(*f)(csPARGS))
{
  size_t id = getId(f);
  execute(id);
}

size_t CSPARALLEL_API csParallelTask::getWorkSize(int idf)
{
    return THREAD_GLOBAL_SIZE[idf];
}

void CSPARALLEL_API csParallelTask::setBufferShape(size_t idf, BUFFER_SHAPE shape)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<csPARGS> parg = BLOCK_ARGS[idf];
  for(size_t i=0; i<nBlocks; i++)
  {
    parg[i].setBounds(shape[i]);
  }
}

void CSPARALLEL_API csParallelTask::setDelay(size_t idf, size_t delay)
{
    size_t nBlocks = BLOCK_ARGS[idf].size();
    for(size_t i=0; i<nBlocks; i++)
    {
        BLOCK_ARGS[idf][i].setDelay(delay);
    }
}

void CSPARALLEL_API csParallelTask::setDelay(size_t idf, vector<size_t> delayList)
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

void CSPARALLEL_API csParallelTask::setBufferShapeRegular(size_t idf, size_t workSize)
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

void CSPARALLEL_API csParallelTask::setExecutionMode(size_t idf, bool execMode)
{
    size_t nBlocks = BLOCK_ARGS[idf].size();
    for(int i=0; i<nBlocks; i++)
    {
        BLOCK_ARGS[idf][i].EXEC_MODE = execMode;
    }
}

void CSPARALLEL_API csParallelTask::setExecutionMode(size_t idf, vector<bool> execMode)
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

void CSPARALLEL_API csParallelTask::updateArg(size_t idf, size_t ida, void*&&arg)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<csPARGS> parg = BLOCK_ARGS[idf];
  for(size_t i=0; i<nBlocks; i++)
  {
    parg[i].setArg(ida,arg);
  }
}

template<size_t N> void CSPARALLEL_API csParallelTask::updateArg(size_t idf, const size_t (&ida)[N], void* const (&arg)[N])
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<csPARGS>parg = BLOCK_ARGS[idf];

  for(size_t i=0; i<nBlocks; i++)
  {
    for(size_t j=0; j<N; j++)
      parg[i].setArg(ida[j],arg[j]);
  }
}

void CSPARALLEL_API csParallelTask::updateArg(size_t idf, initializer_list<size_t>ida, initializer_list<void*> arg)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<csPARGS>parg = BLOCK_ARGS[idf];

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

template<class T>void CSPARALLEL_API csParallelTask::updateArg(size_t idf, size_t ida, T arg)
{
  size_t nBlocks = BLOCK_ARGS[idf].size();
  vector<csPARGS>parg = BLOCK_ARGS[idf];
  for(size_t i=0; i<nBlocks; i++)
  {
    *(T*)parg[i] = arg;
  }
}

