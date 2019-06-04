#pragma once

#include "Utils.h"

#include <memory>

namespace Simulator
{
	class TimeManager
	{
	public:
		typedef std::shared_ptr<TimeManager> ptr;

	private:
		real m_time;
		real m_timeStepSize;
		static TimeManager::ptr m_instance;

	public:
		TimeManager()
		{
			m_time = 0;
			m_timeStepSize = 0.005;
		}

		~TimeManager() = default;

		static TimeManager::ptr getSingleton();
		
		// Getter.
		real getTime() const { return m_time; }
		real getTimeStepSize() const { return m_timeStepSize; }

		// Setter.
		void setTime(real time) { m_time = time; }
		void setTimeStepSize(real step) { m_timeStepSize = step; }
	};
}