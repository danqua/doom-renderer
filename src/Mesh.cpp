#include "Mesh.h"

Mesh CreateMesh(const std::vector<Vertex>& vertices, GLenum drawMode)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, uv));

	Mesh result;
	result.vao = vao;
	result.vbo = vbo;
	result.numVertices = static_cast<int>(vertices.size());
	result.drawMode = drawMode;
	return result;
}

void DestroyMesh(Mesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(1, &mesh.vbo);
	mesh.vao = 0;
	mesh.vbo = 0;
	mesh.numVertices = 0;
}

void Mesh::Draw(const Shader& shader, const glm::mat4& transform) const
{
	if (texture)
	{
		texture->Bind();
	}

	shader.Bind();
	shader.SetUniformMat4(shader.GetUniformLocation("u_model"), transform);
	glBindVertexArray(vao);
	glDrawArrays(drawMode, 0, numVertices);
}