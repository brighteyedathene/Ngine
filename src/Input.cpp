#include "Input.h"



Input::Input()
{
	f_forward = 0;
	f_up = 0;
	f_right = 0;

	f_rx = 0;
	f_ry = 0;
	f_rz = 0;

	f_sx = 1;
	f_sy = 1;
	f_sz = 1;

	translate_delta = 0.02;
	rotate_delta = 0.02;
	scale_delta = 0.01;

}


Input::~Input()
{
}


void Input::Tick()
{
	// TODO change these geto axes
	f_forward = 0;
	f_up = 0;
	f_right = 0;

	// flatten mouse motion deltas
	mouse_xrel = 0;
	mouse_yrel = 0;

	for (std::pair<SDL_Scancode, Button> pair : scancodeMap)
	{
		const char* keyName = SDL_GetKeyName(SDL_GetKeyFromScancode(pair.first));
		//std::cout << keyName << pair.second.downFrame << " ";

		if (pair.second.downFrame)
		{
			scancodeMap[pair.first].downFrame = false;
		}
		if (pair.second.releaseFrame)
		{
			scancodeMap[pair.first].releaseFrame = false;
		}

	}
	//std::cout << std::endl;
}

void Input::SetButtonDown(SDL_Scancode scancode)
{
	if (!scancodeMap[scancode].down)
	{
		scancodeMap[scancode].downFrame = true;
		scancodeMap[scancode].down = true;
	}
	else
	{
		//std::cout << "this button was already down\n";
	}
}

void Input::SetButtonReleased(SDL_Scancode scancode)
{
	scancodeMap[scancode].releaseFrame = true;
	scancodeMap[scancode].down = false;
}


// Getting states from a label
bool Input::GetButton(std::string label)
{
	if (!ButtonExists(label))
		return false;

	bool ret = false;
	for (SDL_Scancode scancode : labelMap[label])
		ret = ret | scancodeMap[scancode].down;

	return ret;
}

bool Input::GetButtonDown(std::string label)
{
	if (!ButtonExists(label))
		return false;

	bool ret = false;
	for (SDL_Scancode scancode : labelMap[label])
		ret = ret | scancodeMap[scancode].downFrame;

	return ret;
}

bool Input::GetButtonReleased(std::string label)
{
	if (!ButtonExists(label))
		return false;

	bool ret = false;
	for (SDL_Scancode scancode : labelMap[label])
		ret = ret | scancodeMap[scancode].releaseFrame;

	return ret;
}

void Input::CreateButtonMapping(std::string label, SDL_Scancode scancode)
{
	labelMap[label].push_back(scancode);
}

bool Input::ButtonExists(std::string label)
{
	std::unordered_map<std::string, std::vector<SDL_Scancode>>::iterator iter = labelMap.find(label);
	if (iter == labelMap.end())
	{
		std::cerr << "didn't find button named " << label << " in labelMap" << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}