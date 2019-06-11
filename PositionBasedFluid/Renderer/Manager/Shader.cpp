#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace Renderer
{
	Shader::Shader(const string &vertexPath, const string &fragmentPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		vertexCode = readShaderFromFile(vertexPath);
		fragmentCode = readShaderFromFile(fragmentPath);
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// shader Program
		m_id = glCreateProgram();
		glAttachShader(m_id, vertex);
		glAttachShader(m_id, fragment);
		glLinkProgram(m_id);
		checkCompileErrors(m_id, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	Shader::Shader(const std::string & vertexPath, const std::string & fragmentPath, const std::string & geometryPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		vertexCode = readShaderFromFile(vertexPath);
		fragmentCode = readShaderFromFile(fragmentPath);
		geometryCode = readShaderFromFile(geometryPath);
		const char *vShaderCode = vertexCode.c_str();
		const char *fShaderCode = fragmentCode.c_str();
		const char *gShaderCode = geometryCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment, geometry;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// geometry shader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
		// shader Program
		m_id = glCreateProgram();
		glAttachShader(m_id, vertex);
		glAttachShader(m_id, fragment);
		glAttachShader(m_id, geometry);
		glLinkProgram(m_id);
		checkCompileErrors(m_id, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteShader(geometry);
	}

	void Shader::checkCompileErrors(unsigned int shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
					<< type << "\n" << infoLog
					<< "\n -- --------------------------------------------------- -- "
					<< std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
					<< type << "\n" << infoLog
					<< "\n -- --------------------------------------------------- -- "
					<< std::endl;
			}
		}
	}

	std::string Shader::readShaderFromFile(const std::string & path)
	{
		std::string code;
		std::ifstream file;

		//! 读取着色器文件.
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			file.open(path);
			std::stringstream stream;
			stream << file.rdbuf();
			file.close();
			code = stream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ:"
				<< path << std::endl;
		}

		return code;
	}
}