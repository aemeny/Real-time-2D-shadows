#include "Window.h"
#include <iostream>

/*
* window deconstructor for memory clean up
*/
Window::~Window()
{
	if (m_context)
	{
		std::cout << "SDL_GL_DeleteContext\n" << std::endl;
		SDL_GL_DeleteContext(m_context);
	}
	if (m_window)
	{
		std::cout << "SDL_DestroyWindow\n" << std::endl;
		SDL_DestroyWindow(m_window);

		std::cout << "SDL_Quit\n" << std::endl;
		SDL_Quit();
	}
}