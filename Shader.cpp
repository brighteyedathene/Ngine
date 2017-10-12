#include "Shader.h"

#include <typeinfo>

#pragma region shader_setup_functions

std::string LoadShaderSource(const GLchar* shaderPath)
{
	/* 
	Retrieve shader source code from shaderPath.
	Remember: Result must be c_str()'d because OpenGL can't compile std::string
	*/
	std::string source;
	std::ifstream file;
	// ensure ifstream objects can throw exceptions
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		file.open(shaderPath);
		std::stringstream stream;
		// read file's buffer  contents into streams
		stream << file.rdbuf();
		// close file handlers
		file.close();
		// convert stream into string
		source = stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Error: shader file wasn't read" << std::endl;
	}

	return source;
}

unsigned int CompileShader(const char* shaderSource, GLenum shaderType)
{
	unsigned int shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error: Shader compilation failed\n" << infoLog << std::endl;
		//std::cout << "this shader's source:\n" << shaderSource << std::endl;
		return -1;
	}
	return shader;
}

unsigned int LinkShaders(unsigned int shaders[], int numShaders)
{
	unsigned int shaderProgram = glCreateProgram();
	for (int i = 0; i < numShaders; i++)
	{
		glAttachShader(shaderProgram, shaders[i]);
	}
	glLinkProgram(shaderProgram);
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error: Shader linking failed\n" << infoLog << std::endl;
		return -1;
	}
	return shaderProgram;
}

#pragma endregion shader_setup_functions


Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	std::string vertexShaderSource = LoadShaderSource(vertexPath);
	std::string fragmentShaderSource = LoadShaderSource(fragmentPath);

	unsigned int vertexShader = CompileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);

	unsigned int shaders[2] = { vertexShader, fragmentShader };
	ID = LinkShaders(shaders, 2);

	// delete shaders (they're in the program now)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}


Shader::~Shader()
{
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetMat4(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}