#pragma once

#include <vector>
#include <iostream>
#include <string>

#include <glad/glad.h>

using std::vector;

class Cubemap
{
public:
	Cubemap();
	~Cubemap();

	unsigned int m_textureID;

	void LoadCubemap(vector<std::string> faces);

};

