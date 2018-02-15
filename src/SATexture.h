#pragma once
#include <iostream>
#include <glad/glad.h>




class SATexture
{
public:
	// Texture ID
	unsigned int ID = 0; // Defaults to 0: glDeleteTextures silently ignores 0's

	// Some defualt
	bool generateMipmap = false;
	GLint paramMin = GL_NEAREST;
	GLint paramMag = GL_NEAREST;

	// Constructor loads image from path and create a stand-alone OpenGL texture
	SATexture(const GLchar* path);
	SATexture();
	~SATexture();

	void LoadFromPath(const GLchar* path, bool generateMipmap, GLint paramMin, GLint paramMag, bool anisotropicFiltering);

	/*Bind this texture to [GL_TEXTURE0 + index]*/
	void Bind(unsigned int index);
};

