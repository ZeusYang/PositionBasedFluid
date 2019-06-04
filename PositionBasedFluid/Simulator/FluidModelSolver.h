#pragma once

#include "Utils.h"
#include "TimeManager.h"
#include "ParticleGroup.h"
#include "NeighborSearch.h"

namespace Simulator
{
	class FluidModelSolver
	{
	protected:
		real m_density0;
		real m_viscosity;
		real m_sphRadius;
		real m_particleRadius;
		unsigned int m_maxIter;
		ParticleGroup m_particles;
		std::vector<real> m_density;
		std::vector<real> m_lambda;
		std::vector<glm::vec3> m_deltaPos;
		NeighborSearch *m_neighborSearch;
		TimeManager::ptr m_timeMgr;

	public:
		FluidModelSolver();
		~FluidModelSolver();

		// Setup/Cleanup.
		void clearModel();
		void setupModel(
			const std::vector<glm::vec3> &fluidParticles,
			const std::vector<glm::vec3> &boundaryParticles);

		// Getter.
		real getDensity0() const { return m_density0; }
		real getViscosity() const { return m_viscosity; }
		real getSphRadius() const { return m_sphRadius; }
		ParticleGroup &getParticles() { return m_particles; }
		real getParticleRadius() const { return m_particleRadius; }
		real getTimeStepSize() const { return m_timeMgr->getTimeStepSize(); }
		unsigned int getFluidParticleNum() const { return m_lambda.size(); }
		unsigned int getTotalParticleNum() const { return m_particles.getNumOfParticles(); }
		std::vector<glm::vec3> &getParticleGroup() { return m_particles.getPositionGroup(); }

		// Setter.
		void setDensity0(const real &value) { m_density0 = value; }
		void setViscosity(const real &value) { m_viscosity = value; }
		void setSphRadius(const real &value) { m_sphRadius = value; }
		void setParticleRadius(const real &value);

		// Simulation.
		void simulate();

	protected:
		// Setting.
		void resetParticles();
		void releaseParticles();
		void initMasses(const unsigned int &nFluidParticles);
		void resizeParticles(const unsigned int &nFluidParticles, const unsigned int &nBoundaryParticles);

		// Solver
		void clearAccelerations();
		void computeDensities();
		void computeXSPHViscosity();
		void constraintProjection();
		void computeVorticityConfinement();
		void computeSurfaceTension();
		void updateTimeStepSizeCFL(const real &minTimeStep, const real &maxTimeStep);

	};
}

