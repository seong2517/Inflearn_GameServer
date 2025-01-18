#pragma once
#include <stack>
#include <mutex>
using namespace std;


template<typename T>
class LockStack
{
public:
	LockStack() { }
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_Stack.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_Stack.empty() == true)
			return false;

		value = std::move(_Stack.top());
		_Stack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] { return _Stack.empty() == false; });

		value = std::move(_Stack.top());
		_Stack.pop();
	}

private:
	stack<T> _Stack;
	mutex		 _mutex;
	condition_variable _condVar;
};
