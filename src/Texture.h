#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <assimp/Importer.hpp>

using namespace std;

struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;

	Texture();
	bool LoadFromFile(const char *file, const string &directory);

	void Bind(GLenum TextureUnit);
};