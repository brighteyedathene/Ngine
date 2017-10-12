#pragma once

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	// shader ID
	unsigned int ID;

	//constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	
	// destructor does... 
	~Shader();

	void Use();

	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetMat4(const std::string &name, glm::mat4 value) const;

};

