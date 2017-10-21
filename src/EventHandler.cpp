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

		case SDL_MOUSEMOTION:
			MouseMotionTick(e);
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
		break;

	case SDL_KEYUP:
		p_input->SetButtonReleased(e.key.keysym.scancode);
		break;
	}
}

void EventHandler::MouseMotionTick(SDL_Event e)
{
	p_input->mouse_x = e.motion.x;
	p_input->mouse_y = e.motion.y;
	p_input->mouse_xrel = e.motion.xrel;
	p_input->mouse_yrel = e.motion.yrel;

	//std::cout << "pos     " << e.motion.x << ", " << e.motion.y;
	//std::cout << "       rel     " << e.motion.xrel << ", " << e.motion.yrel << std::endl;
}