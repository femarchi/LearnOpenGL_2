#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// program ID
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath) 
	{
		ID = createShaderProgram(vertexPath, fragmentPath);
	}

	void use()
	{
		glUseProgram(ID);
	};

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	};

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	};

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	};

private:

	void checkShaderCompilation(unsigned int shader, const char* type)
	{
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
			std::cout << "ERROR::SHADER::"<< type <<"::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	void checkProgramLink(unsigned int program)
	{
		int success;
		char infoLog[512];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		}
	}

	unsigned int createShader(const char* shaderPath, const char* type) 
	{
		// read shader file from path
		std::string shaderCode;
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			shaderFile.open(shaderPath);
			std::stringstream shaderStream;
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();
			shaderCode = shaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* shaderSource = shaderCode.c_str();

		// create a shader
		unsigned int shader;
		if (type == "VERTEX")
			shader = glCreateShader(GL_VERTEX_SHADER);
		else
			shader = glCreateShader(GL_FRAGMENT_SHADER);
		// source the code into the created shader and compile
		glShaderSource(shader, 1, &shaderSource, NULL);
		glCompileShader(shader);
		checkShaderCompilation(shader, type);
		return shader;
	}

	unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath) 
	{
		unsigned int vertexShader = createShader(vertexPath, "VERTEX");
		unsigned int fragmentShader = createShader(fragmentPath, "FRAGMENT");
		// create a program to link the vertex and fragment shaders
		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		checkProgramLink(shaderProgram);

		// after shaders are linked we can delete them
		glDeleteShader(shaderProgram);
		glDeleteShader(fragmentShader);

		return shaderProgram;
	}
};

#endif
