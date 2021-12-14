#pragma once
#include <glad/glad.h>
#include <string>

struct Texture
{
	GLuint texture;
	int width;
	int height;

	void Bind(int slot = 0) const;
	void Unbind() const;
};

Texture CreateTexture(const void* pixels, int width, int height);
Texture CreateTextureFromFile(const std::string& filename);
void DestroyTexture(Texture& texture);