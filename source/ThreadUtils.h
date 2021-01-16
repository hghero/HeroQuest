#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H

#include <QtCore/QMutex>

class AutoMutex
{
public:
	AutoMutex(QMutex& mutex);
	~AutoMutex();

private:
	QMutex& _mutex;
};

#endif
