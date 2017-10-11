#pragma once

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// shader ID
	unsigned int ID;

	//constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();
};

