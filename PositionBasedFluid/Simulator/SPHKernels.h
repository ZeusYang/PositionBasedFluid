#pragma once

#include "Utils.h"
#include <glm/glm.hpp>
#include <iostream>

namespace Simulator
{
	class SPHKernels
	{
	protected:
		static real m_radius;
		static real m_zeroW;
		static real m_radius2;
		static real m_poly6Cof;
		static real m_spikyCof;

	public:
		static void setRadius(const real &value)
		{
			m_radius = value;
			m_radius2 = value * value;
			m_zeroW = poly6WKernel(glm::vec3(0.0f));
			m_poly6Cof = 315.0 / (64.0 * M_PI * pow(value, 9.0));
			m_spikyCof = -45.0 / (M_PI * pow(value, 6.0));
		}
		
		static real getRadius()
		{
			return m_radius;
		}

		static real getZeroWKenel()
		{
			return m_zeroW;
		}

		// poly6 kernel function.
		static real poly6WKernel(const glm::vec3 &r);
		// spiky kernel's gradient function.
		static glm::vec3 spikyWKernelGrad(const glm::vec3 &r);


	};

}

