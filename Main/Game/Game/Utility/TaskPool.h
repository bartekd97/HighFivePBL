#pragma once

#include <array>
#include <thread>
#include <future>
#include <concurrent_queue.h>

class ITaskPool
{
public:
	static const size_t MAX_WORKERS = 8;
	typedef std::function<void()> Task;
	enum class Priority
	{
		IDLE,
		LOWEST,
		BELOW_NORMAL,
		NORMAL,
		ABOVE_NORMAL,
		HIGHEST
	};

	inline virtual size_t WorkersCount() = 0;
	inline virtual std::thread& GetWorker(size_t index) = 0;
	inline virtual void AddTask(Task task) = 0;
	inline virtual void FillWorkers(std::function<void()> func) = 0;
	inline virtual void WaitForAll() = 0;

protected:
	void SetPriority(ITaskPool::Priority priority);
};

template <size_t _WorkersCount, ITaskPool::Priority _Priority = ITaskPool::Priority::NORMAL>
class TaskPool : public ITaskPool
{
private:
	concurrency::concurrent_queue<Task> tasks;
	std::array<std::thread, _WorkersCount> workers;
	std::uint32_t business;

	bool keepAlive = true;
	void Work(std::uint32_t bitflag)
	{
		Task task;
		while (keepAlive)
		{
			while (!tasks.empty())
			{
				business |= bitflag;
				if (tasks.try_pop(task)) {
					task();
				}
			}
			business ^= bitflag;
			std::this_thread::yield();
		}
	}

public:
	inline TaskPool()
	{
		assert(_WorkersCount <= MAX_WORKERS && "Too many workers");
		for (uint32_t i = 0; i < workers.size(); i++)
		{
			workers[i] = std::thread(&TaskPool<_WorkersCount, _Priority>::Work, this, 1 << i);
		}
		SetPriority(_Priority);
		business = 0;
	}
	inline ~TaskPool()
	{
		keepAlive = false;
		for (uint32_t i = 0; i < workers.size(); i++)
		{
			workers[i].join();
		}
	}

	inline virtual size_t WorkersCount() { return _WorkersCount; }
	inline virtual std::thread& GetWorker(size_t index) { return workers[index]; }
	inline virtual void AddTask(Task task) { tasks.push(std::move(task)); };
	inline virtual void FillWorkers(std::function<void()> func) {
		for (uint32_t i = 0; i < workers.size(); i++)
			AddTask(Task(func));
	}
	inline virtual void WaitForAll() {
		while (business != 0 || !tasks.empty()) {
			std::this_thread::yield();
		}
	};
};

