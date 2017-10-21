#pragma once

#include <vector>
#include <SDL2\SDL.h>
#include <unordered_map>
#include <string>

#include <iostream>

class Button
{
public:
	bool downFrame;
	bool releaseFrame;
	bool down;

	Button()
	{
		this->downFrame = false;
		this->releaseFrame = false;
		this->down = false;
	}
};

class Input
{
public:
	
	// position of mouse
	int mouse_x, mouse_y;
	// movement of mouse this frame
	int mouse_xrel, mouse_yrel;


	float f_forward;
	float f_up;
	float f_right;

	float f_rx;
	float f_ry;
	float f_rz;

	float f_sx;
	float f_sy;
	float f_sz;

	float translate_delta;
	float rotate_delta;
	float scale_delta;

	Input();
	~Input();

	/* Removes downFrames and releaseFrames
	Call this either before EventHandler.tick() or after processing all input for the frame
	*/
	void Tick();

	void SetButtonDown(SDL_Scancode scancode);
	void SetButtonReleased(SDL_Scancode scancode);


	bool GetButton(std::string label);
	bool GetButtonDown(std::string label);
	bool GetButtonReleased(std::string label);

	// Adds scancode to the list of SDL_Scancodes associate with label
	void CreateButtonMapping(std::string label, SDL_Scancode scancode);


private:
	std::unordered_map<std::string, std::vector<SDL_Scancode>> labelMap;
	std::unordered_map<SDL_Scancode, Button> scancodeMap;

	bool ButtonExists(std::string label);

};

