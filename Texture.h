#pragma once
#include <iostream>
#include <glad/glad.h>




class Texture
{
public:
	// Texture ID
	unsigned int ID;

	// Texture index
	// from GL_TEXTURE0 -> GL_TEXTURE31 (or GL_TEXTURE15)
	// This will be added to GL_TEXTURE0
	int textureIndex; 

	// Constructor loads image from path and create an OpenGL texture
	Texture(const GLchar* path, int textureIndex);
	~Texture();

	void Use();
};

