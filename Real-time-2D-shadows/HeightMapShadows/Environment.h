#pragma once
#include <memory>

struct Core;

/*
*  Environment struct, used to store and calculate delta time
*/
struct Environment
{
public:
	Environment(std::weak_ptr<Core> _core);
	~Environment();
	void init();
	void tick(); // Updates delta time
	double getDT(); // Returns delta time
private:
	double last;
	double curr;
	double deltaTime;

	// Core pointer reference
	std::weak_ptr<Core> m_core;
};


