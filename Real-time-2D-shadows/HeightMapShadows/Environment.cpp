#include "Environment.h"
#include <Windows.h>
#include "Core.h"

/*
* constructor, resets delta time values and get the first tick count
*/
Environment::Environment(std::weak_ptr<Core> _core)
	: last(0), curr(0), deltaTime(0), m_core(_core)
{
	init();
}


Environment::~Environment() {}


/*
* saves initial tick count
*/
void Environment::init()
{
#ifdef _WIN32
	last = GetTickCount();
#else
	struct timeval tv = { 0 };
	gettimeofday(&tv, NULL);
	last = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
#endif
}


/*
* tick called every frame to update the current delta time depending on time taken by the last frame
*/
void Environment::tick()
{
#ifdef _WIN32
	DWORD curr = GetTickCount();
#else
	struct timeval tv = { 0 };
	gettimeofday(&tv, NULL);
	double curr = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
#endif
	double diff = curr - last;
	deltaTime = diff / 1000.0f;
	last = curr;
}


/*
* returns delta time, if delta time is 0 as of rounding it gets set to 0.000001f to save math errors
*/
double Environment::getDT()
{
	if (deltaTime == 0)
	{
		return 0.0001f;
	}
	return deltaTime;
}
