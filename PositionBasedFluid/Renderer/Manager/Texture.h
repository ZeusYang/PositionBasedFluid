#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace Renderer
{
	class Texture
	{
	protected:
		std::string m_name;
		unsigned int m_id;

	public:
		typedef std::shared_ptr<Texture> ptr;

		Texture() = default;
		virtual ~Texture() = default;

		virtual void bind(unsigned int unit) = 0;
		virtual void unBind() = 0;

		unsigned int getTextureId() { return m_id; }

	private:
		virtual void setupTexture(const std::string &path, const std::string &pFix) = 0;
		virtual void clearTexture() = 0;

	};

	class Texture2D : public Texture
	{
	private:
		glm::vec4 m_borderColor;
		int m_width, m_height, m_channel;

	public:
		typedef std::shared_ptr<Texture2D> ptr;

		Texture2D(const std::string &path, glm::vec4 bColor = glm::vec4(1.0f));
		~Texture2D();

		virtual void bind(unsigned int unit);
		virtual void unBind();

	private:
		virtual void setupTexture(const std::string &path, const std::string &pFix);
		virtual void clearTexture();
	};

	class TextureCube : public Texture
	{
	public:
		TextureCube(const std::string &path, const std::string &postFix);
		~TextureCube();

		virtual void bind(unsigned int unit);
		virtual void unBind();

	private:
		virtual void setupTexture(const std::string &path, const std::string &pFix);
		virtual void clearTexture();
	};

	class TextureDepth : public Texture
	{
	private:
		int m_width, m_height;

	public:
		TextureDepth(int width, int height);
		~TextureDepth();

		virtual void bind(unsigned int unit);
		virtual void unBind();

	private:
		virtual void setupTexture(const std::string &path, const std::string &pFix);
		virtual void clearTexture();
	};

}

