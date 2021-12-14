#include "Shader.h"
#include <fstream>
#include <sstream>

static GLuint CreateShaderType(GLenum type, const char* code)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);

	int status = 0;
	char message[1024];

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (!status) {
		glGetShaderInfoLog(shader, 1024, NULL, message);
		printf(type == GL_VERTEX_SHADER ? "[Vertex Shader]\n" : "[Fragment Shader]\n");
		printf("%s\n", message);
	}
	return shader;
}

static GLuint CreateProgram(GLuint vs, GLuint fs) {
	GLuint program = glCreateProgram();

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	int status = 0;
	char message[1024];

	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (!status) {
		glGetProgramInfoLog(program, 1024, NULL, message);
		printf("[Shader Program] %s\n", message);
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


Shader CreateShaderFromFile(const std::string& filename)
{
	std::ifstream fs(filename, std::ios::binary);
	std::stringstream ss;

	ss << fs.rdbuf();

	std::string line;
	std::string vscode;
	std::string fscode;
	std::string* strPointer = nullptr;

	while (std::getline(ss, line))
	{
		if (line.find("#vertex") != std::string::npos)
		{
			strPointer = &vscode;
		}
		else if (line.find("fragment") != std::string::npos)
		{
			strPointer = &fscode;
		}
		else
		{
			if (strPointer)
			{
				*strPointer += line + '\n';
			}
		}
	}

	return CreateShader(vscode, fscode);
}

Shader CreateShader(const std::string& vscode, const std::string& fscode)
{
	GLuint vs = CreateShaderType(GL_VERTEX_SHADER, vscode.c_str());
	GLuint fs = CreateShaderType(GL_FRAGMENT_SHADER, fscode.c_str());
	GLuint program = CreateProgram(vs, fs);
	return Shader { program };
}

void DestroyShader(Shader& shader)
{
	if (glIsProgram(shader.id))
	{
		glDeleteProgram(shader.id);
		shader.id = 0;
	}
}

void Shader::Bind() const
{
	glUseProgram(id);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

GLint Shader::GetUniformLocation(const std::string& name) const
{
	return glGetUniformLocation(id, name.c_str());
}

GLint Shader::GetAttributeLocation(const std::string& name) const
{
	return glGetAttribLocation(id, name.c_str());
}

void Shader::SetUniformInt(int location, int value) const
{
	glUniform1i(location, value);
}

void Shader::SetUniformFloat(int location, float value) const
{
	glUniform1f(location, value);
}

void Shader::SetUniformVec2(int location, const glm::vec2& value) const
{
	glUniform2f(location, value.x, value.y);
}

void Shader::SetUniformVec3(int location, const glm::vec3& value) const
{
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetUniformVec4(int location, const glm::vec4& value) const
{
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::SetUniformMat4(int location, const glm::mat4& value) const
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}
