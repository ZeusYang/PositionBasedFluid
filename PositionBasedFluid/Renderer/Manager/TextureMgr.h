#pragma once

#include <map>
#include <vector>

#include "Texture.h"
#include "Singleton.h"

namespace Renderer
{
	class TextureMgr : public Singleton<TextureMgr>
	{
	private:
		std::vector<Texture::ptr> m_units;
		std::map<std::string, unsigned int> m_unitMap;

	public:
		typedef std::shared_ptr<TextureMgr> ptr;

		TextureMgr() = default;
		~TextureMgr() = default;

		static TextureMgr::ptr getSingleton();

		unsigned int loadTexture2D(const std::string &name, const std::string &path, glm::vec4 bColor = glm::vec4(1.0f))
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new Texture2D(path, bColor));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTextureCube(const std::string &name, const std::string &path, const std::string &pFix)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new TextureCube(path, pFix));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadTextureDepth(const std::string &name, int width, int height)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Texture::ptr tex(new TextureDepth(width, height));
			m_units.push_back(tex);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		Texture::ptr getTexture(const std::string &name)
		{
			if (m_unitMap.find(name) == m_unitMap.end())
				return nullptr;
			return m_units[m_unitMap[name]];
		}

		Texture::ptr getTexture(unsigned int unit)
		{
			if (unit >= m_units.size())
				return nullptr;
			return m_units[unit];
		}

		bool bindTexture(unsigned int index, unsigned int unit)
		{
			if (index >= m_units.size())
				return false;
			m_units[index]->bind(unit);
			return true;
		}

		bool unBindTexture(unsigned int index)
		{
			if (index >= m_units.size())
				return false;
			m_units[index]->unBind();
			return true;
		}
	};
}

