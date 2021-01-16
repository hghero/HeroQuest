#include "ThreadUtils.h"

AutoMutex::AutoMutex(QMutex& mutex)
: _mutex(mutex)
{
	_mutex.lock();
}

AutoMutex::~AutoMutex()
{
    _mutex.unlock();
}
