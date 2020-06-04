#include "TaskPool.h"
#include "Logger.h"
#include <Windows.h>

void ITaskPool::SetPriority(ITaskPool::Priority priority)
{
	int win32priority;
	switch (priority)
	{
	case ITaskPool::Priority::IDLE: win32priority = THREAD_PRIORITY_IDLE; break;
	case ITaskPool::Priority::LOWEST: win32priority = THREAD_PRIORITY_LOWEST; break;
	case ITaskPool::Priority::BELOW_NORMAL: win32priority = THREAD_PRIORITY_BELOW_NORMAL; break;
	case ITaskPool::Priority::NORMAL: win32priority = THREAD_PRIORITY_NORMAL; break;
	case ITaskPool::Priority::ABOVE_NORMAL: win32priority = THREAD_PRIORITY_ABOVE_NORMAL; break;
	case ITaskPool::Priority::HIGHEST: win32priority = THREAD_PRIORITY_HIGHEST; break;

	default: win32priority = THREAD_PRIORITY_BELOW_NORMAL; break;
	}

	for (uint32_t i = 0; i < WorkersCount(); i++)
	{
		SetThreadPriority(GetWorker(i).native_handle(), win32priority);
	}
}

