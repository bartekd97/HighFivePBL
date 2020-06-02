#pragma once

#include <thread>

// its very simple and it IS NOT full thread-safely!!!
// it works only with 2 threads where one is making object busy/free and second is waiting for it
struct ThreadBusiness
{
	bool _state = false;

	inline bool IsBusy() { return _state; }
	inline void MakeBusy() { _state = true; }
	inline void MakeFree() { _state = false; }
	inline void Wait() { while (_state) std::this_thread::yield(); }
};