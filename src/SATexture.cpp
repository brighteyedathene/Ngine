#include "SATexture.h"

#include <cassert>


/*
For image loading
Including stb_image in Texture.h caused some errors
I think it was being compiled twice (once for each include of texture.h?)
*/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


SATexture::SATexture()
{
	// Empty constructor
	// Use this to declare changing textures
	// Use LoadFromPath to load images into this SATexture object
}

SATexture::SATexture(const GLchar* path)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	// set wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// load the texture from path
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 3);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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


SATexture::~SATexture()
{
}

void SATexture::Bind(unsigned int index)
{
	assert(index >= 0 && index <= 31);
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void SATexture::LoadFromPath(const GLchar* path, bool generateMipmap, GLint paramMin, GLint paramMag, bool anisotropicFiltering)
{
	glDeleteTextures(1, &ID);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	// set wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, paramMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, paramMag);

	// load the texture from path
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		if (generateMipmap)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		if (anisotropicFiltering)
		{
			GLfloat fLargest;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
		}
	}
	else
	{
		std::cout << "Failed to load texture from " << path << std::endl;
	}
	stbi_image_free(data);

	// unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);
}