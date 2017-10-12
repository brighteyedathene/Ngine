#include "Texture.h"

// for image loading
/*
Including stb_image in Texture.h caused some errors
I think it was being comiled twice (once for each include?)
*/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const GLchar* path, int textureIndex)
{
	// set the texture index so that VAO's can have more tahn one available texture
	this->textureIndex = textureIndex;

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + textureIndex);
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

void Texture::Use()
{
	glActiveTexture(GL_TEXTURE0 + this->textureIndex);
	glBindTexture(GL_TEXTURE_2D, ID);
}