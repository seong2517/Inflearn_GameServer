#pragma once
#include <queue>
#include <mutex>
using namespace std;


template<typename T>
class LockQueue
{
public:
	LockQueue() { }
	LockQueue(const LockQueue&) = delete;

	LockQueue& operator=(const LockQueue&) = delete;

public:
	void Enqueue(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		_Queue.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryDequeue(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_Queue.empty() == true)
			return false;

		value = std::move(_Queue.front());
		_Queue.pop();
		return true;
	}
	
	void WaitDequeue(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] { return _Queue.empty() == false; });

		value = std::move(_Queue.front());
		_Queue.pop();
	}

private:
	queue<T> _Queue;
	mutex		 _mutex;
	condition_variable _condVar;
};
