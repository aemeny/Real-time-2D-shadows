#pragma once
#include <SDL2/SDL.h>

/*
*  Window struct, used to store SDL window information
*/
struct Window
{
	SDL_Window* m_window;
	SDL_GLContext m_context;

	int m_windowHeight;
	int	m_windowWidth;

	~Window();
};

