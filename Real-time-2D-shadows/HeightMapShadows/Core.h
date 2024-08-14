#pragma once
#include "Window.h"
#include "Environment.h"
#include "Input.h"
#include "Noise.h"
#include "QuadRenderer.h"

#include <stdexcept>

struct Core
{
	//! Core constructor.
   /*!
	 The default constructor is not used as instead a initalize function is used,
	 this way i can return a shared pointer reference of core to my main to hold on to.
   */
	Core();
	~Core();

	static std::shared_ptr<Core> initialize();

	void run();
	void stop();

	std::shared_ptr<Window> m_nativeWindow;
	std::shared_ptr<Input> m_input;
	std::shared_ptr<Noise> m_noise;
	std::shared_ptr<QuadRenderer> m_quadRenderer;

	bool m_gameOver;

private:
	void startTransition();

	bool m_running;
	bool m_shrink;
	bool m_textureReady;
	std::weak_ptr<Core> m_self;
	std::shared_ptr<Environment> m_environment;

	std::vector<std::thread> m_threads;
	int m_numOfThreads;
};

