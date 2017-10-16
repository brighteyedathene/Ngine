#pragma once

#include <SDL2/SDL.h>

#include "Display.h"
#include "Input.h"

class EventHandler
{
public:
	EventHandler(Display* p_display, Input* p_input);
	~EventHandler();

	void Tick();

private:
	Display* p_display;
	Input* p_input;

	void WindowTick(SDL_Event e);
	void KeyboardTick(SDL_Event e);

	void MouseMotionTick(SDL_Event e);
	void MouseButtonTick(SDL_Event e);
	void MouseWheelTick(SDL_Event e);


};

