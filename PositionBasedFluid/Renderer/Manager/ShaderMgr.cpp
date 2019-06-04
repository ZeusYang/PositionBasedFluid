#include "ShaderMgr.h"

namespace Renderer
{
	template<> ShaderMgr::ptr Singleton<ShaderMgr>::_instance = nullptr;

	ShaderMgr::ptr ShaderMgr::getSingleton()
	{
		if (_instance == nullptr)
			return _instance = std::make_shared<ShaderMgr>();
		return _instance;
	}
}