#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture CreateTextureFromFile(const std::string& filename)
{
	int x, y, c;
	stbi_set_flip_vertically_on_load(1);

	stbi_uc* pixels = stbi_load(filename.c_str(), &x, &y, &c, STBI_rgb);

	if (!pixels)
	{
		x = 1;
		y = 1;
		pixels = (stbi_uc*)malloc(3);
		pixels[0] = 255;
		pixels[1] = 0;
		pixels[2] = 255;
	}

	Texture result = CreateTexture(pixels, x, y);

	stbi_image_free(pixels);

	return result;
}

Texture CreateTexture(const void* pixels, int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	Texture result;
	result.texture = texture;
	result.width = width;
	result.height = height;
	return result;
}

void DestroyTexture(Texture& texture)
{
	if (glIsTexture(texture.texture))
	{
		glDeleteTextures(1, &texture.texture);
		texture.texture = 0;
		texture.width = 0;
		texture.height = 0;
	}
}


void Texture::Bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}