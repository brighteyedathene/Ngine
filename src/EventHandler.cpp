#include "EventHandler.h"

#include <iostream>


EventHandler::EventHandler(Display* p_display, Input* p_input)
{
	this->p_display = p_display;
	this->p_input = p_input;
}


EventHandler::~EventHandler()
{
}

void EventHandler::Tick()
{
	
	SDL_Event e;
	//SDL_WaitEvent(&e);
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			p_display->Close();
			break;

		case SDL_WINDOWEVENT:
			WindowTick(e);
			break;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			KeyboardTick(e);
			break;
		}
	}
}

void EventHandler::WindowTick(SDL_Event e)
{
	switch (e.window.event)
	{
	case SDL_WINDOWEVENT_RESIZED:
		p_display->ResizeViewPort();
		break;
	}
}

void EventHandler::KeyboardTick(SDL_Event e)
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		p_input->SetButtonDown(e.key.keysym.scancode);
		//std::cout << "Key down!" << std::endl;
		break;

	case SDL_KEYUP:
		p_input->SetButtonReleased(e.key.keysym.scancode);
		//std::cout << "Key up!" << std::endl;
		break;


	}
}