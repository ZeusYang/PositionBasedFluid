#pragma once

#include "Mesh.h"
#include "Singleton.h"

#include <map>
#include <vector>

namespace Renderer
{
	class MeshMgr : public Singleton<MeshMgr>
	{
	private:
		std::vector<Mesh::ptr>  m_units;

	public:
		typedef std::shared_ptr<MeshMgr> ptr;

		static MeshMgr::ptr getSingleton();

		unsigned int loadMesh(Mesh *mesh)
		{
			Mesh::ptr mptr(mesh);
			m_units.push_back(mptr);
			return m_units.size() - 1;
		}

		Mesh::ptr getMesh(unsigned int unit)
		{
			if (unit >= m_units.size())
				return nullptr;
			return m_units[unit];
		}

		bool drawMesh(unsigned int unit, bool instance, int amount = 0)
		{
			if (unit >= m_units.size())
				return false;
			m_units[unit]->draw(instance, amount);
			return true;
		}

	};
}