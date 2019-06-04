#include "TimeManager.h"

namespace Simulator
{
	TimeManager::ptr TimeManager::m_instance = nullptr;

	TimeManager::ptr TimeManager::getSingleton()
	{
		if (m_instance == nullptr)
			m_instance = std::shared_ptr<TimeManager>(new TimeManager());
		return m_instance;
	}

}