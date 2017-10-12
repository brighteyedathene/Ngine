#include "Texture.h"

#include <cassert>


/*
For image loading
Including stb_image in Texture.h caused some errors
I think it was being compiled twice (once for each include of texture.h?)
*/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const GLchar* path)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	// set wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load the texture from path
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture from " << path << std::endl;
	}
	stbi_image_free(data);

	// unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);
}


Texture::~Texture()
{
}

void Texture::Bind(unsigned int index)
{
	assert(index >= 0 && index <= 31);
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, ID);
}