#pragma once

#include <SDL2/SDL.h>

struct Clock
{

	double deltaTime;
	double time;
	Uint64 NOW;
	Uint64 LAST;

	void Init()
	{
		NOW = SDL_GetPerformanceCounter();
		LAST = SDL_GetPerformanceCounter();
		time = 0;
	}

	void Tick()
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		deltaTime = (double)(NOW - LAST) / (double)SDL_GetPerformanceFrequency();
		time += deltaTime;
	}
};

static Clock gameclock;