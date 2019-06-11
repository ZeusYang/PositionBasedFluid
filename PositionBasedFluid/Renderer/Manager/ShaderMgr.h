#pragma once

#include <map>
#include <vector>

#include "Shader.h"
#include "Singleton.h"

namespace Renderer
{
	class ShaderMgr : public Singleton<ShaderMgr>
	{
	private:
		std::vector<Shader::ptr> m_units;
		std::map<std::string, unsigned int> m_unitMap;

	public:
		typedef std::shared_ptr<ShaderMgr> ptr;

		ShaderMgr() = default;
		~ShaderMgr() = default;

		static ShaderMgr::ptr getSingleton();

		unsigned int loadShader(const std::string &name, const std::string &vPath, const std::string &fPath)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Shader::ptr shader(new Shader(vPath, fPath));
			m_units.push_back(shader);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		unsigned int loadShader(const std::string &name, const std::string &vPath, const std::string &fPath,
			const std::string &gPath)
		{
			if (m_unitMap.find(name) != m_unitMap.end())
				return m_unitMap[name];
			Shader::ptr shader(new Shader(vPath, fPath, gPath));
			m_units.push_back(shader);
			m_unitMap[name] = m_units.size() - 1;
			return m_units.size() - 1;
		}

		Shader::ptr getShader(const std::string &name)
		{
			if (m_unitMap.find(name) == m_unitMap.end())
				return nullptr;
			return m_units[m_unitMap[name]];
		}

		Shader::ptr getShader(unsigned int unit)
		{
			if (unit >= m_units.size())
				return nullptr;
			return m_units[unit];
		}

		bool bindShader(unsigned int unit)
		{
			if (unit >= m_units.size())
				return false;
			m_units[unit]->bind();
			return false;
		}

		bool unBindShader()
		{
			Shader::unBind();
			return true;
		}
	};

}

