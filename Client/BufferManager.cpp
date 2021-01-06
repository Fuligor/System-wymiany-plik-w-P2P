#include "BufferManager.h"

#include "FileFrag.h"

BufferManager::BufferManager()
    :QObject()
{
}

BufferManager* BufferManager::getInstance()
{
    if(sigleInstance == nullptr)
    {
        sigleInstance = new BufferManager();
    }

    return sigleInstance;
}

void BufferManager::onDataCleared(FileFrag* fragment)
{
    for(std::list <FileFrag*>::iterator i = bufferedFrags.begin(); i != bufferedFrags.end(); ++i)
    {
        if(*i == fragment)
        {
            bufferSpace += bufferedFrags.front()->getSize();
            bufferedFrags.erase(i);
            break;
        }
    }
}

void BufferManager::onDataBuffered(FileFrag* fragment)
{
    bufferedFrags.push_back(fragment);
    bufferSpace -= fragment->getSize();

    while(bufferSpace < 0 && bufferedFrags.size() > 1)
    {
        bufferedFrags.front()->free();
    }
}

BufferManager* BufferManager::sigleInstance = nullptr;
QMutex BufferManager::mutex;
