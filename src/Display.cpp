#include "Display.h"

#include <glad\glad.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>

// Kills the program with an error message
void SDL_Die(const char* message)
{
	fprintf(stderr, "%s: %s\n", message, SDL_GetError());
	exit(2);
}

Display::Display(int width, int height, const std::string& title)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// Sets some paramters upon SDL_CreateWindow when using SDL_WINDOW_OPENGL
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	// Use v-sync
	SDL_GL_SetSwapInterval(1);

	// TODO fullscreen and shit
	m_window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL |
		SDL_WINDOW_RESIZABLE 
	);
	if (m_window == NULL)
		SDL_Die("Failed to create SDL window");

	m_glContext = SDL_GL_CreateContext(m_window);
	if (m_glContext == NULL)
		SDL_Die("Failed to create OpenGL context");

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
		SDL_Die("Failed to link with glad");

	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	// opengl also needs to know the dimensions of the window
	glViewport(0, 0, width, height);
	m_aspect = (float) width / height;

	glEnable(GL_DEPTH_TEST);

	m_isClosed = false;
}

void Display::Clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Display::~Display()
{
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Display::Update()
{
	SDL_GL_SwapWindow(m_window);
}

void Display::Close()
{
	m_isClosed = true;
}

bool Display::IsClosed()
{
	return m_isClosed;
}
float Display::Aspect()
{
	return m_aspect;
}

// resize the viewport using values obtained from SDL
void Display::ResizeViewPort()
{
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	glViewport(0, 0, width, height);
	m_aspect = (float) width / height;
}

void Display::GetWindowSize(int *w, int* h)
{
	SDL_GetWindowSize(m_window, w, h);
}