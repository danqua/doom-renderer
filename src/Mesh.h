#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Mesh
{
	GLuint vbo;
	GLuint vao;
	int numVertices;
	GLenum drawMode;
	Texture* texture = nullptr;

	void Draw(const Shader& shader, const glm::mat4& transform) const;
};

Mesh CreateMesh(const std::vector<Vertex>& vertices, GLenum drawMode = GL_TRIANGLES);
void DestroyMesh(Mesh& mesh);