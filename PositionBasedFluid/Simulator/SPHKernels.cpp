#include "SPHKernels.h"

namespace Simulator
{
	real SPHKernels::m_radius;
	real SPHKernels::m_zeroW;
	real SPHKernels::m_radius2;
	real SPHKernels::m_poly6Cof;
	real SPHKernels::m_spikyCof;

	real SPHKernels::poly6WKernel(const glm::vec3 & r)
	{
		//real ret = 0.0;
		//real rl = glm::length(r);
		//if (rl > m_radius)
		//	return ret;
		//real rl2 = rl * rl;
		//real d = m_radius2 - rl2;
		//ret = m_poly6Cof * d * d * d;

		real ret = 0.0;
		real rl = glm::length(r);
		real q = rl / m_radius;
		real h3 = m_radius * m_radius * m_radius;
		if (q <= 0.5)
		{
			real q2 = q * q;
			real q3 = q2 * q;
			ret = 8.0 / (M_PI * h3) * (6.0 * q3 - 6.0 * q2 + 1.0);
		}
		else
		{
			ret = 16.0 / (M_PI * h3) * pow(1 - q, 3.0);
		}
		return ret;
	}

	glm::vec3 SPHKernels::spikyWKernelGrad(const glm::vec3 & r)
	{
		//glm::vec3 ret(0.0f);
		//real rl = glm::length(r);
		//if (rl > m_radius)
		//	return ret;
		//real d = m_radius - rl;
		//ret = static_cast<float>(m_spikyCof * d * d / rl) * r;

		glm::vec3 ret(0.0f);
		real rl = glm::length(r);
		real q = rl / m_radius;
		real h3 = m_radius * m_radius * m_radius;
		if (rl > 1.0e-6)
		{
			const glm::vec3 gradq = static_cast<float>(1.0 / (rl * m_radius)) * r;
			if (q <= 0.5)
			{
				ret = static_cast<float>(48.0 / (M_PI * h3) * q * (3.0 * q - 2.0)) * gradq;
			}
			else
			{
				real factor = 1.0 - q;
				ret = static_cast<float>(48.0 / (M_PI * h3) * (-factor * factor)) * gradq;
			}
		}
		return ret;
	}

}