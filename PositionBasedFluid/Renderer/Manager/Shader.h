#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace Renderer
{

	class Shader
	{
	private:
		unsigned int m_id;

	public:
		typedef std::shared_ptr<Shader> ptr;

		Shader(const std::string &vertexPath, const std::string &fragmentPath);

		Shader(const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath);

		~Shader() { glDeleteProgram(m_id); }

		void bind() { glUseProgram(m_id); }

		static void unBind() { glUseProgram(0); }

		// utility uniform functions
		// ------------------------------------------------------------------------
		void setBool(const std::string &name, bool value) const
		{
			glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<int>(value));
		}

		// ------------------------------------------------------------------------
		void setInt(const std::string &name, int value) const
		{
			glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
		}

		// ------------------------------------------------------------------------
		void setFloat(const std::string &name, float value) const
		{
			glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
		}

		// ------------------------------------------------------------------------
		void setVec2(const std::string &name, const glm::vec2 &value) const
		{
			glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
		}
		void setVec2(const std::string &name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
		}

		// ------------------------------------------------------------------------
		void setVec3(const std::string &name, const glm::vec3 &value) const
		{
			glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
		}
		void setVec3(const std::string &name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
		}

		// ------------------------------------------------------------------------
		void setVec4(const std::string &name, const glm::vec4 &value) const
		{
			glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
		}
		void setVec4(const std::string &name, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
		}

		// ------------------------------------------------------------------------
		void setMat2(const std::string &name, const glm::mat2 &mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		// ------------------------------------------------------------------------
		void setMat3(const std::string &name, const glm::mat3 &mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		// ------------------------------------------------------------------------
		void setMat4(const std::string &name, const glm::mat4 &mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

	private:
		void checkCompileErrors(unsigned int shader, std::string type);
		std::string readShaderFromFile(const std::string &path);
	};

}
