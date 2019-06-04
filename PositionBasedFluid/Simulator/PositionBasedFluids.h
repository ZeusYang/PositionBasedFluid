#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Utils.h"

namespace Simulator
{
	class PositionBasedFluids
	{
	public:

		// semi-implicit Euler time integration.
		static void semiImplicitEuler(
			const real &h,
			const real &mass,
			glm::vec3 &position,
			glm::vec3 &velocity,
			const glm::vec3 &acceleration
		);

		static void velocityUpdateFirstOrder(
			const real &h,
			const real &mass,
			const glm::vec3 &position,
			const glm::vec3 &oldPosition,
			glm::vec3 &velocity
		);

		// calculate particle's fluid density using SPH method.
		static bool computeFluidDensity(
			const unsigned int &particleIndex,
			const unsigned int &numFluidParticle,
			const std::vector<glm::vec3> &position,
			const std::vector<real> &mass,
			const std::vector<unsigned int> &neighors,
			const real &density0,
			real &density_err,
			real &density
		);

		// calculate particle's lagrange multiplier.
		static bool computeLagrangeMultiplier(
			const unsigned int &particleIndex,
			const unsigned int &numFluidParticle,
			const std::vector<glm::vec3> &position,
			const std::vector<real> &mass,
			const std::vector<unsigned int> &neighors,
			const real &density,
			const real &density0,
			real &lambda
		);

		// perform a density constraint.
		static bool solveDensityConstraint(
			const unsigned int &particleIndex,
			const unsigned int &numFluidParticle,
			const std::vector<glm::vec3> &position,
			const std::vector<real> &mass,
			const std::vector<unsigned int> &neighors,
			const std::vector<real> &lamba,
			const real &sphRadius,
			const real &density0,
			glm::vec3 &deltaPos
		);

	};

}

