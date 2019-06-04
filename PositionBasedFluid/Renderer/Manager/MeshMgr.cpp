#include "MeshMgr.h"

namespace Renderer
{
	template<> MeshMgr::ptr Singleton<MeshMgr>::_instance = nullptr;

	MeshMgr::ptr MeshMgr::getSingleton()
	{
		if (_instance == nullptr)
			return _instance = std::make_shared<MeshMgr>();
		return _instance;
	}
}