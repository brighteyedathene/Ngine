#pragma once
#include <iostream>
#include <glad/glad.h>




class SATexture
{
public:
	// Texture ID
	unsigned int ID;

	// Constructor loads image from path and create a stand-alone OpenGL texture
	SATexture(const GLchar* path);
	~SATexture();

	/*Bind this texture to [GL_TEXTURE0 + index]*/
	void Bind(unsigned int index);
};

