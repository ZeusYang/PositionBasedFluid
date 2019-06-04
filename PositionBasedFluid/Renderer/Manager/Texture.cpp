#include "Texture.h"

#include <vector>
#include <iostream>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Renderer
{
	Texture2D::Texture2D(const std::string & path, glm::vec4 bColor)
	{
		m_borderColor = bColor;
		setupTexture(path, "");
	}

	Texture2D::~Texture2D()
	{
		clearTexture();
	}

	void Texture2D::bind(unsigned int unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void Texture2D::unBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::setupTexture(const std::string & path, const std::string &pFix)
	{
		// texture unit generation.
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		// filter setting.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &m_borderColor[0]);
		// load the image.
		unsigned char *data = stbi_load(path.c_str(), &m_width, &m_height, &m_channel, 0);
		GLenum format;
		if (m_channel == 1)
			format = GL_RED;
		else if (m_channel == 3)
			format = GL_RGB;
		else if (m_channel == 4)
			format = GL_RGBA;
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height,
				0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture at path:" << path << std::endl;
		}
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::clearTexture()
	{
		glDeleteTextures(1, &m_id);
	}

	TextureCube::TextureCube(const std::string & path, const std::string & postFix)
	{
		setupTexture(path, postFix);
	}

	TextureCube::~TextureCube()
	{
		clearTexture();
	}

	void TextureCube::bind(unsigned int unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	}

	void TextureCube::unBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::setupTexture(const std::string & path, const std::string & pFix)
	{
		// load cube map.
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		int width, height, nrComponents;
		// 6 faces.
		std::vector<std::string> faces =
		{
			path + "right" + pFix,
			path + "left" + pFix,
			path + "top" + pFix,
			path + "bottom" + pFix,
			path + "front" + pFix,
			path + "back" + pFix
		};
		// load the image step by step.
		for (int x = 0; x < faces.size(); ++x)
		{
			unsigned char *data = stbi_load(faces[x].c_str(), &width, &height,
				&nrComponents, 0);
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, format,
					width, height, 0, format, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " 
					<< faces[x] << std::endl;
			}
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::clearTexture()
	{
		glDeleteTextures(1, &m_id);
	}

	TextureDepth::TextureDepth(int width, int height)
		:m_width(width), m_height(height)
	{
		setupTexture("", "");
	}

	TextureDepth::~TextureDepth()
	{
		clearTexture();
	}

	void TextureDepth::bind(unsigned int unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void TextureDepth::unBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextureDepth::setupTexture(const std::string & path, const std::string & pFix)
	{
		// generate depth buffer.
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextureDepth::clearTexture()
	{
		glDeleteTextures(1, &m_id);
	}

}