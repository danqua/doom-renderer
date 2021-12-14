#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>

struct Shader
{
	GLuint id;

	void Bind() const;
	void Unbind() const;
	GLint GetUniformLocation(const std::string& name) const;
	GLint GetAttributeLocation(const std::string& name) const;
	void SetUniformInt(int location, int value) const;
	void SetUniformFloat(int location, float value) const;
	void SetUniformVec2(int location, const glm::vec2& value) const;
	void SetUniformVec3(int location, const glm::vec3& value) const;
	void SetUniformVec4(int location, const glm::vec4& value) const;
	void SetUniformMat4(int location, const glm::mat4& value) const;
};

Shader CreateShaderFromFile(const std::string& filename);

Shader CreateShader(const std::string& vscode, const std::string& fscode);

void DestroyShader(Shader& shader);