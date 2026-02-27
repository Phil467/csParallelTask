#include "csPargs.h"

std::mutex _mutex;

CSPARGS::CSPARGS(size_t _nbArgs)
{
    init(_nbArgs);
};

void CSPARGS::init(size_t _nbArgs)
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

void* CSPARGS::getArg(size_t i)
{
    return Args[i+2];
}

CSPARGS::BOUNDS CSPARGS::getBounds()
{
    return bounds;
}

void CSPARGS::setBounds(CSPARGS::BOUNDS b)
{
    bounds = b;

}

void CSPARGS::setWorkSize(size_t _workSize)
{
    workSize = _workSize;

}

size_t CSPARGS::getWorkSize()
{
    return workSize;
}

size_t CSPARGS::getBlockId()
{
    return blockId;
}

size_t CSPARGS::getArgNumber()
{
    return nbArgs;
}

void CSPARGS::setArg(size_t i, void*arg)
{
    Args[i+2] = arg;
}

void CSPARGS::setDelay(size_t _delay)
{
    delay = _delay;
}

void CSPARGS::sleepHour()
{
    std::this_thread::sleep_for(std::chrono::hours(delay));
}

void CSPARGS::sleepMin()
{
    std::this_thread::sleep_for(std::chrono::minutes(delay));
}

void CSPARGS::sleepSec()
{
    std::this_thread::sleep_for(std::chrono::seconds(delay));
}

void CSPARGS::sleepMilli()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void CSPARGS::sleepMicro()
{
    std::this_thread::sleep_for(std::chrono::microseconds(delay));
}

void CSPARGS::sleepNano()
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
}

void CSPARGS::setBlockId(size_t id)
{
    blockId = id;
}

void CSPARGS::setBlocksNumber(size_t _blocksNumber)
{
  blocksNumber = _blocksNumber;
}

size_t CSPARGS::getBlocksNumber()
{
  return blocksNumber;
}

void CSPARGS::getBounds(size_t workSize, size_t*min, size_t*max)
{
  size_t delta = workSize/blocksNumber;
  *min = blockId*delta;
  if(blockId == blocksNumber-1)
    *max = workSize;
  else
    *max = (blockId + 1)*delta;
}


void CSPARGS::setArgNumber(size_t _nbArgs)
{
    nbArgs = _nbArgs;
    int m = nbArgs + 2;
    Args = (void**)realloc(Args, m*sizeof(void*));
}


void CSPARGS::regArgs(void*arg,...)
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

template<size_t _nbArgs> void CSPARGS::regArgs2(void*args[_nbArgs])
{
    for (int i=0 ; i<_nbArgs ; i++)
    {
        Args[i+2] = args[i];
    }
}

void CSPARGS::regArgs2(void**args, size_t nbArgs)
{
    for (int i=0 ; i<nbArgs ; i++)
    {
        Args[i+2] = args[i];
    }
}

void CSPARGS::lockGuard()
{
    std::lock_guard<std::mutex> lock(_mutex);
}

void CSPARGS::clear()
{
    free(Args);
    Args = 0;
    bounds = {0};
    blockId = 0;
    workSize = 0;
}

CSPARGS::operator CSPARGS::BOUNDS()
{
    return bounds;
}

CSPARGS::operator size_t()
{
    return blockId;
};

void* CSPARGS::operator[](size_t i)
{
    return Args[i+2];
};

void CSPARGS::operator=(csVOID_ARG va)
{
    Args[va.i+2] = va.arg;
};
