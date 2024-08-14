#include "Core.h"

Core::Core() {}

Core::~Core() {}

void Core::run()
{
	m_running = true;
	bool firstNoiseGen = true;
	
	while (m_running) // Main loop
	{
		//Clear depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw background
		glClearColor(0.384f, 0.8f, 0.9f, 1);

		// Call tick
		m_environment->tick();

		m_input->tick();

		if (m_input->isKeyPressed(SDLK_ESCAPE)) // When escape is pressed break out of game loop
		{
			break;
		}

		if (m_input->isMouseClicked() || firstNoiseGen)
		{
			m_shrink = true;
			m_quadRenderer->m_shader->setanimate(false);
			firstNoiseGen = false;

			startTransition();
		}
		if (m_shrink)
		{
			if (m_quadRenderer->m_shader->shrinkIsland(m_environment->getDT()))
			{
				for (size_t i = 0; i < m_numOfThreads; i++)
				{
					m_threads[i].join();
				}
				m_noise->setCurrentThread(-1);
				m_shrink = false;
				m_quadRenderer->m_shader->setanimate(true);
				m_quadRenderer->setHeightMapTexture(std::make_shared<Texture>(&m_noise->getHeightMap(), m_nativeWindow->m_windowWidth, m_nativeWindow->m_windowHeight));
				m_textureReady = true;
			}
		}

		m_quadRenderer->m_shader->setShaderValues(m_input->getMousePos(), m_nativeWindow->m_windowWidth, m_nativeWindow->m_windowHeight, (float)m_environment->getDT());

		if (m_textureReady)
		{
			m_quadRenderer->renderQuad();
		}
		
		//Clear pressed/released keys for this frame
		m_input->clear();

		//Swap window
		SDL_GL_SwapWindow(m_nativeWindow->m_window);
	}
}

void Core::stop()
{
	m_running = false;
}

void Core::startTransition()
{
	m_noise->setSeed((float)SDL_GetTicks() / 1000.0f);

	m_noise->setnumOfThreads(m_numOfThreads);

	for (size_t i = 0; i < m_numOfThreads; i++)
	{
		m_threads[i] = std::thread([this] { this->m_noise->generatePerlinTexture(); });
	}
}

std::shared_ptr<Core> Core::initialize()
{
	std::shared_ptr<Core> rtn = std::make_shared<Core>();

	//const int SCREEN_WIDTH = 1920;
	//const int SCREEN_HEIGHT = 1080;
	
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 800;

	rtn->m_nativeWindow = std::make_shared<Window>();
	rtn->m_nativeWindow->m_windowWidth = SCREEN_WIDTH;
	rtn->m_nativeWindow->m_windowHeight = SCREEN_HEIGHT;

	// Window Initialise
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::runtime_error("Failed to initialize SDL");
	}

	if (!(rtn->m_nativeWindow->m_window = SDL_CreateWindow("HeightMapShadows",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		rtn->m_nativeWindow->m_windowWidth, rtn->m_nativeWindow->m_windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)))
	{
		SDL_Quit();
		throw std::runtime_error("Failed to create Window");
	}

	if (!(rtn->m_nativeWindow->m_context = SDL_GL_CreateContext(rtn->m_nativeWindow->m_window)))
	{
		SDL_DestroyWindow(rtn->m_nativeWindow->m_window);
		SDL_Quit();
		throw std::runtime_error("Failed to create OpenGL context");
	}

	// Init glew
	glewInit();

	// Turn vsync on for SDL
	SDL_GL_SetSwapInterval(1);

	// Control cursor
	SDL_ShowCursor(true);

	//Init variables
	rtn->m_input = std::make_shared<Input>(rtn);

	rtn->m_environment = std::make_shared<Environment>(rtn);
	rtn->m_noise = std::make_shared<Noise>(rtn, rtn->m_nativeWindow->m_windowWidth, rtn->m_nativeWindow->m_windowHeight);

	rtn->m_quadRenderer = std::make_shared<QuadRenderer>("../Shaders/vertexShader.glsl",
		"../Shaders/fragmentShader.glsl", "../Textures/Voronoi2.jpeg", "../Textures/RandomNoise.png");

	rtn->m_self = rtn;
	rtn->m_gameOver = false;
	rtn->m_shrink = false;
	rtn->m_textureReady = false;

	rtn->m_numOfThreads = 8;
	rtn->m_threads = std::vector<std::thread>(rtn->m_numOfThreads);

	return rtn;
}
