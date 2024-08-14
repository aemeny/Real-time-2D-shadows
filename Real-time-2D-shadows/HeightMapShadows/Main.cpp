#include "Core.h"

#undef main
int main()
{
	// Core init
	std::shared_ptr<Core> core = Core::initialize();

	core->run();

	return 0;
}

