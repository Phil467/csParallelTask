#include "csPARGS.h"

std::mutex _mutex;

csPARGS::csPARGS(size_t _nbArgs)
{
    init(_nbArgs);
};

void csPARGS::init(size_t _nbArgs)
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
void* csPARGS::getArg(size_t i)
{
    return Args[i+2];
}
csPARGS::BOUNDS csPARGS::getBounds()
{
    return bounds;
}
void csPARGS::setBounds(csPARGS::BOUNDS b)
{
    bounds = b;

}
void csPARGS::setWorkSize(size_t _workSize)
{
    workSize = _workSize;

}

size_t csPARGS::getWorkSize()
{
    return workSize;
}

size_t csPARGS::getBlockId()
{
    return blockId;
}

size_t csPARGS::getArgNumber()
{
    return nbArgs;
}

void csPARGS::setArg(size_t i, void*arg)
{
    Args[i+2]=arg;
}

void csPARGS::setDelay(size_t _delay)
{
    delay = _delay;
}

void csPARGS::sleepHour()
{
    std::this_thread::sleep_for(std::chrono::hours(delay));
}

void csPARGS::sleepMin()
{
    std::this_thread::sleep_for(std::chrono::minutes(delay));
}

void csPARGS::sleepSec()
{
    std::this_thread::sleep_for(std::chrono::seconds(delay));
}

void csPARGS::sleepMilli()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void csPARGS::sleepMicro()
{
    std::this_thread::sleep_for(std::chrono::microseconds(delay));
}

void csPARGS::sleepNano()
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
}

void csPARGS::setBlockId(size_t id)
{
    blockId = id;
}

void csPARGS::setBlocksNumber(size_t _blocksNumber)
{
  blocksNumber = _blocksNumber;
}

size_t csPARGS::getBlocksNumber()
{
  return blocksNumber;
}

void csPARGS::getBounds(size_t workSize, size_t*min, size_t*max)
{
  size_t delta = workSize/blocksNumber;
  *min = blockId*delta;
  if(blockId == blocksNumber-1)
    *max = workSize;
  else
    *max = (blockId + 1)*delta;
}


void csPARGS::setArgNumber(size_t _nbArgs)
{
    nbArgs = _nbArgs;
    int m = nbArgs + 2;
    Args = (void**)realloc(Args, m*sizeof(void*));

}


void csPARGS::makeArgs(void*arg,...)
{
    va_list adArgs ;
    void* parv;
    va_start (adArgs, arg);
    Args[2]=arg;
    for (int i=0 ; i<nbArgs ; i++)
    {
        parv = va_arg (adArgs, void*) ;
        Args[i+3]=parv;
    }
    va_end(adArgs);
}

template<size_t _nbArgs> void csPARGS::makeArgs2(void*args[_nbArgs])
{
    for (int i=0 ; i<_nbArgs ; i++)
    {
        Args[i+2]=args[i];
    }
}

void csPARGS::makeArgs2(void**args, size_t nbArgs)
{
    for (int i=0 ; i<nbArgs ; i++)
    {
        Args[i+2]=args[i];
    }
}

void csPARGS::lockGuard()
{
    std::lock_guard<std::mutex> lock(_mutex);
}

void csPARGS::clear()
{
    free(Args);
    Args = 0;
    bounds = {0};
    blockId = 0;
    workSize = 0;
}

csPARGS::operator csPARGS::BOUNDS()
{
    return bounds;
}

csPARGS::operator size_t()
{
    return blockId;
};

void* csPARGS::operator[](size_t i)
{
    return Args[i+2];
};

void csPARGS::operator=(csVOID_ARG va)
{
    Args[va.i+2]=va.arg;
};
