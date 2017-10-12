#pragma once
#include <iostream>
#include <glad/glad.h>




class Texture
{
public:
	// Texture ID
	unsigned int ID;

	// Constructor loads image from path and create an OpenGL texture
	Texture(const GLchar* path);
	~Texture();

	void Bind(unsigned int index);
};

