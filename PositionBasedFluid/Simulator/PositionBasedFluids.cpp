#include "PositionBasedFluids.h"

#include <algorithm>

#include "SPHKernels.h"

namespace Simulator
{
	void PositionBasedFluids::semiImplicitEuler(
		const real & h,
		const real & mass,
		glm::vec3 & position,
		glm::vec3 & velocity,
		const glm::vec3 & acceleration)
	{
		if (mass != 0)
		{
			velocity += acceleration * static_cast<float>(h);
			position += velocity * static_cast<float>(h);
		}
	}

	void PositionBasedFluids::velocityUpdateFirstOrder(
		const real & h, 
		const real & mass,
		const glm::vec3 & position,
		const glm::vec3 & oldPosition,
		glm::vec3 & velocity)
	{
		if (mass != 0.0)
			velocity = static_cast<float>(1.0 / h) * (position - oldPosition);
	}

	bool PositionBasedFluids::computeFluidDensity(
		const unsigned int& particleIndex,
		const unsigned int& numFluidParticle,
		const std::vector<glm::vec3>& position,
		const std::vector<real>& mass,
		const std::vector<unsigned int>& neighbors,
		const real & density0,
		real & density_err,
		real & density)
	{
		density = mass[particleIndex] * SPHKernels::getZeroWKenel();
		for (unsigned int x = 0; x < neighbors.size(); ++x)
		{
			unsigned int neighborIndex = neighbors[x];
			if (neighborIndex < numFluidParticle)
			{
				density += mass[neighborIndex] * SPHKernels::poly6WKernel(
					position[particleIndex] - position[neighborIndex]);
			}
			else
			{
				density += mass[neighborIndex] * SPHKernels::poly6WKernel(
					position[particleIndex] - position[neighborIndex]);
			}
		}
		density_err = std::max(density, density0) - density0;
		return true;
	}

	bool PositionBasedFluids::computeLagrangeMultiplier(
		const unsigned int& particleIndex,
		const unsigned int& numFluidParticle,
		const std::vector<glm::vec3>& position,
		const std::vector<real>& mass,
		const std::vector<unsigned int>& neighbors,
		const real & density,
		const real & density0,
		real & lambda)
	{
		const real eps = 1.0e-6;
		const real constraint = std::max(density / density0 - 1.0, 0.0);
		//const real constraint = density / density0 - 1.0;
		if (constraint != 0.0)
		{
			real sum_grad_cj = 0.0;
			glm::vec3 grad_ci(0.0);
			for (unsigned int x = 0; x < neighbors.size(); ++x)
			{
				unsigned int neighborIndex = neighbors[x];
				if (neighborIndex < numFluidParticle)
				{
					glm::vec3 grad_cj = static_cast<float>(+mass[neighborIndex] / density0)
						* SPHKernels::spikyWKernelGrad(position[particleIndex] - position[neighborIndex]);
					//real len = glm::length(grad_cj);
					sum_grad_cj += pow(glm::length(grad_cj), 2.0);
					grad_ci += grad_cj;
				}
				else
				{
					glm::vec3 grad_cj = static_cast<float>(+mass[neighborIndex] / density0)
						* SPHKernels::spikyWKernelGrad(position[particleIndex] - position[neighborIndex]);
					//real len = glm::length(grad_cj);
					sum_grad_cj += pow(glm::length(grad_cj), 2.0);
					grad_ci += grad_cj;	
				}
			}
			sum_grad_cj += pow(glm::length(grad_ci), 2.0);
			lambda = -constraint / (sum_grad_cj + eps);
		}
		else
			lambda = 0.0;
		return true;
	}

	bool PositionBasedFluids::solveDensityConstraint(
		const unsigned int& particleIndex,
		const unsigned int& numFluidParticle,
		const std::vector<glm::vec3>& position,
		const std::vector<real>& mass,
		const std::vector<unsigned int>& neighbors,
		const std::vector<real>& lambda,
		const real &sphRadius,
		const real &density0,
		glm::vec3 &deltaPos)
	{
		deltaPos = glm::vec3(0.0f);
		for (unsigned int x = 0; x < neighbors.size(); ++x)
		{
			unsigned int neighborIndex = neighbors[x];
			if (neighborIndex < numFluidParticle)
			{
				glm::vec3 grad_cj = static_cast<float>(mass[neighborIndex] / density0) 
					* SPHKernels::spikyWKernelGrad(position[particleIndex] - position[neighborIndex]);
				deltaPos += static_cast<float>(lambda[particleIndex] + lambda[neighborIndex]) * grad_cj;
			}
			else
			{
				glm::vec3 grad_cj = static_cast<float>(mass[neighborIndex] / density0)
					* SPHKernels::spikyWKernelGrad(position[particleIndex] - position[neighborIndex]);
				deltaPos += static_cast<float>(lambda[particleIndex]) * grad_cj;
			}
		}
		return true;
	}

}
