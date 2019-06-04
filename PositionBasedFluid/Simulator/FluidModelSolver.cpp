#include "FluidModelSolver.h"

#include "SPHKernels.h"
#include "PositionBasedFluids.h"

#include <algorithm>
#include <tbb/parallel_for.h>

using namespace tbb;

namespace Simulator
{
	FluidModelSolver::FluidModelSolver()
	{
		m_maxIter = 5;
		m_density0 = 1000.0;
		m_viscosity = 0.02;
		//m_viscosity = 0.5;
		m_particleRadius = 0.025;
		m_neighborSearch = nullptr;
	}

	FluidModelSolver::~FluidModelSolver()
	{
		clearModel();
	}

	void FluidModelSolver::setupModel(
		const std::vector<glm::vec3> &fluidParticles,
		const std::vector<glm::vec3> &boundaryParticles)
	{
		unsigned int nFluidPartiles = fluidParticles.size();
		unsigned int nBoundaryParticles = boundaryParticles.size();

		// time manager.
		if (m_timeMgr == nullptr)
			m_timeMgr = TimeManager::getSingleton();

		// reset
		releaseParticles();
		SPHKernels::setRadius(m_sphRadius);
		resizeParticles(nFluidPartiles, nBoundaryParticles);

		parallel_for(blocked_range<size_t>(0, nFluidPartiles, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				m_particles.setRestPosition(x, fluidParticles[x]);
			}
		}
		);
		//for (unsigned int x = 0; x < nFluidPartiles; ++x)
		//{
		//	m_particles.setRestPosition(x, fluidParticles[x]);
		//}

		parallel_for(blocked_range<size_t>(0, nBoundaryParticles, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				m_particles.setRestPosition(x + nFluidPartiles, boundaryParticles[x]);
			}
		});
		//for (unsigned int x = 0; x < nBoundaryParticles; ++x)
		//{
		//	m_particles.setRestPosition(x + nFluidPartiles, boundaryParticles[x]);
		//}

		initMasses(nFluidPartiles);

		// calculate boundary's properties.
		NeighborSearch neighborhood(nBoundaryParticles, m_sphRadius);
		neighborhood.neighborhoodSearch(boundaryParticles, 0, nBoundaryParticles);
		parallel_for(blocked_range<size_t>(0, nBoundaryParticles, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				std::vector<unsigned int> &neighbors = neighborhood.getNeighbors(x);
				real delta = SPHKernels::getZeroWKenel();
				for (unsigned int y = 0; y < neighbors.size(); ++y)
				{
					unsigned int neighborIndex = neighbors[y];
					delta += SPHKernels::poly6WKernel(boundaryParticles[x] 
						- boundaryParticles[neighborIndex]);
				}
				delta = m_density0 / delta;
				m_particles.setMass(x + nFluidPartiles, delta);
			}
		}
		);
		//for (unsigned int x = 0; x < nBoundaryParticles; ++x)
		//{
		//	std::vector<unsigned int> &neighbors = neighborhood.getNeighbors(x);
		//	real delta = SPHKernels::getZeroWKenel();
		//	for (unsigned int y = 0; y < neighbors.size(); ++y)
		//	{
		//		unsigned int neighborIndex = neighbors[y];
		//		delta += SPHKernels::poly6WKernel(boundaryParticles[x] - boundaryParticles[neighborIndex]);
		//	}
		//	delta = 1.0 / delta;
		//	m_particles.setMass(x + nFluidPartiles, m_density0 * delta);
		//}

		// neighborhood searcher.
		if (m_neighborSearch == nullptr)
			m_neighborSearch = new NeighborSearch(nFluidPartiles, m_sphRadius);
		m_neighborSearch->setRadius(m_sphRadius);

		resetParticles();
	}

	void FluidModelSolver::clearModel()
	{
		m_particles.release();
		std::vector<real>().swap(m_lambda);
		std::vector<real>().swap(m_density);
		std::vector<glm::vec3>().swap(m_deltaPos);
		if (m_neighborSearch)delete m_neighborSearch;
		m_neighborSearch = nullptr;
	}

	void FluidModelSolver::initMasses(const unsigned int &nFluidParticles)
	{
		real diameter = 2.0 * m_particleRadius;
		real volume = diameter * diameter * diameter * 0.8;
		real mass = volume * m_density0;
		parallel_for(blocked_range<size_t>(0, nFluidParticles, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				m_particles.setMass(x, mass);
			}
		}
		);
		//for (unsigned int x = 0; x < nFluidParticles; ++x)
		//{
		//	m_particles.setMass(x, volume * m_density0);
		//}
	}

	void FluidModelSolver::clearAccelerations()
	{
		const glm::vec3 gravity(0.0f, -9.81f, 0.0f);
		//m_particles.getAccelerationGroup().assign(gravity);
		parallel_for(blocked_range<size_t>(0, m_lambda.size(), 10000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				if (m_particles.getMass(x) != 0.0)
				{
					m_particles.setAcceleration(x, gravity);
				}
			}
		});
		//for (unsigned int x = 0; x < m_lambda.size(); ++x)
		//{
		//	if (m_particles.getMass(x) != 0.0)
		//	{
		//		m_particles.setAcceleration(x, gravity);
		//	}
		//}
	}

	void FluidModelSolver::computeDensities()
	{
		unsigned int nFluidParticles = m_lambda.size();
		for (unsigned int x = 0; x < nFluidParticles; ++x)
		{
			std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
			real density = 0.0;
			real density_err = 0.0;
			PositionBasedFluids::computeFluidDensity(x, nFluidParticles, m_particles.getPositionGroup(),
				m_particles.getMassGroup(), neighbors, m_density0, density_err, density);
			m_density[x] = density;
		}
	}

	void FluidModelSolver::computeXSPHViscosity()
	{
		unsigned int nFluidParticles = m_lambda.size();
		//parallel_for(blocked_range<size_t>(0, nFluidParticles, 1000),
		//	[&](blocked_range<size_t> &r)
		//{
		//	for (size_t x = r.begin(); x != r.end(); ++x)
		//	{
		//		glm::vec3 &velocity = m_particles.getVelocity(x);
		//		const glm::vec3 &position = m_particles.getPosition(x);
		//		std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
		//		glm::vec3 sum_value(0.0f);
		//		for (unsigned int y = 0; y < neighbors.size(); ++y)
		//		{
		//			unsigned int neighborIndex = neighbors[y];
		//			if (neighborIndex < nFluidParticles)
		//			{
		//				const real density_j = m_density[neighborIndex];
		//				const glm::vec3 &position_j = m_particles.getPosition(neighborIndex);
		//				const glm::vec3 &velocity_j = m_particles.getVelocity(neighborIndex);
		//				sum_value -=
		//					static_cast<float>(
		//					m_viscosity * (m_particles.getMass(neighborIndex) / density_j))
		//					* (velocity - velocity_j) 
		//					* static_cast<float>(SPHKernels::poly6WKernel(position - position_j));
		//				//glm::vec3 tmp = velocity - velocity_j;
		//				//tmp *= SPHKernels::poly6WKernel(position - position_j)
		//				//	* (m_particles.getMass(neighborIndex) / density_j);
		//				//sum_value += tmp;
		//			}
		//		}
		//		//sum_value *= m_viscosity;
		//		m_particles.setVelocity(x, velocity + sum_value);
		//	}
		//});
		for (unsigned int x = 0; x < nFluidParticles; ++x)
		{
			glm::vec3 &velocity = m_particles.getVelocity(x);
			const glm::vec3 &position = m_particles.getPosition(x);
			std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
			glm::vec3 sum_value(0.0f);
			for (unsigned int y = 0; y < neighbors.size(); ++y)
			{
				unsigned int neighborIndex = neighbors[y];
				if (neighborIndex < nFluidParticles)
				{
					const real density_j = m_density[neighborIndex];
					const glm::vec3 &position_j = m_particles.getPosition(neighborIndex);
					const glm::vec3 &velocity_j = m_particles.getVelocity(neighborIndex);
					glm::vec3 tmp = velocity - velocity_j;
					tmp *= SPHKernels::poly6WKernel(position - position_j) 
						* (m_particles.getMass(neighborIndex) / density_j);
					sum_value -= tmp;
				}
			}
			sum_value *= m_viscosity;
			m_particles.setVelocity(x, velocity + sum_value);
		}
	}

	void FluidModelSolver::constraintProjection()
	{
		unsigned int iter = 0;
		unsigned int nFluidParticles = m_lambda.size();

		while (iter < m_maxIter)
		{
			// calculate density and lagrange multiplier.
			parallel_for(blocked_range<size_t>(0, nFluidParticles, 1000),
				[&](blocked_range<size_t> &r)
			{
				for (size_t x = r.begin(); x != r.end(); ++x)
				{
					real density_err;
					std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
					PositionBasedFluids::computeFluidDensity
					(
						x,
						nFluidParticles,
						m_particles.getPositionGroup(),
						m_particles.getMassGroup(),
						neighbors,
						m_density0,
						density_err,
						m_density[x]
					);
					PositionBasedFluids::computeLagrangeMultiplier
					(
						x,
						nFluidParticles,
						m_particles.getPositionGroup(),
						m_particles.getMassGroup(),
						neighbors,
						m_density[x],
						m_density0,
						m_lambda[x]
					);
				}
			});
			//for (int x = 0; x < nFluidParticles; ++x)
			//{
			//	real density_err;
			//	std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
			//	PositionBasedFluids::computeFluidDensity
			//	(
			//		x,
			//		nFluidParticles,
			//		m_particles.getPositionGroup(),
			//		m_particles.getMassGroup(),
			//		neighbors,
			//		m_density0,
			//		density_err,
			//		m_density[x]
			//	);
			//	PositionBasedFluids::computeLagrangeMultiplier
			//	(
			//		x,
			//		nFluidParticles,
			//		m_particles.getPositionGroup(),
			//		m_particles.getMassGroup(),
			//		neighbors,
			//		m_density[x],
			//		m_density0,
			//		m_lambda[x]
			//	);
			//}

			// perform density constraint.
			parallel_for(blocked_range<size_t>(0, nFluidParticles, 1000),
				[&](blocked_range<size_t> &r)
			{
				for (size_t x = r.begin(); x != r.end(); ++x)
				{
					std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
					PositionBasedFluids::solveDensityConstraint
					(
						x,
						nFluidParticles,
						m_particles.getPositionGroup(),
						m_particles.getMassGroup(),
						neighbors,
						m_lambda,
						m_sphRadius,
						m_density0,
						m_deltaPos[x]
					);
				}
			});
			//for (unsigned int x = 0; x < nFluidParticles; ++x)
			//{
			//	std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
			//	PositionBasedFluids::solveDensityConstraint
			//	(
			//		x,
			//		nFluidParticles,
			//		m_particles.getPositionGroup(),
			//		m_particles.getMassGroup(),
			//		neighbors,
			//		m_lambda,
			//		m_density0,
			//		m_deltaPos[x]
			//	);
			//}

			// add the delta position to particles' position.
			parallel_for(blocked_range<size_t>(0, nFluidParticles, 10000),
				[&](blocked_range<size_t> &r)
			{
				for (size_t x = r.begin(); x != r.end(); ++x)
				{
					//m_particles.setPosition(x, m_particles.getPosition(x) + m_deltaPos[x]);
					m_particles.getPosition(x) += m_deltaPos[x];
				}
			});
			//for (unsigned int x = 0; x < nFluidParticles; ++x)
			//{
			//	m_particles.setPosition(x, m_particles.getPosition(x) + m_deltaPos[x]);
			//}
			
			++iter;
		}
	}

	void FluidModelSolver::computeVorticityConfinement()
	{
		unsigned int nFluidParticles = m_lambda.size();
		static bool test = true;
		glm::vec3 record(-FLT_MAX);
		std::vector<glm::vec3> deltaVelocity(nFluidParticles, glm::vec3(0.0f));
		for (unsigned int x = 0; x < nFluidParticles; ++x)
		{
			std::vector<unsigned int> &neighbors = m_neighborSearch->getNeighbors(x);
			glm::vec3 N(0.0f);
			glm::vec3 curl(0.0f);
			glm::vec3 curlX(0.0f);
			glm::vec3 curlY(0.0f);
			glm::vec3 curlZ(0.0f);
			const glm::vec3 &velocity_i = m_particles.getVelocity(x);
			const glm::vec3 &position_i = m_particles.getPosition(x);
			for (unsigned int y = 0; y < neighbors.size(); ++y)
			{
				unsigned int neighborIndex = neighbors[y];
				if (neighborIndex >= nFluidParticles)
					continue;
				const glm::vec3 velGap = m_particles.getVelocity(neighborIndex) - velocity_i;
				curl += glm::cross(velGap, SPHKernels::spikyWKernelGrad(position_i - 
					m_particles.getPosition(neighborIndex)));
				curlX += glm::cross(velGap, SPHKernels::spikyWKernelGrad(position_i + glm::vec3(0.01, 0.0, 0.0)
					- m_particles.getPosition(neighborIndex)));
				curlY += glm::cross(velGap, SPHKernels::spikyWKernelGrad(position_i + glm::vec3(0.0, 0.01, 0.0)
					- m_particles.getPosition(neighborIndex)));
				curlZ += glm::cross(velGap, SPHKernels::spikyWKernelGrad(position_i + glm::vec3(0.0, 0.0, 0.01)
					- m_particles.getPosition(neighborIndex)));
				//if (test)
				//{
				//	glm::vec3 t = SPHKernels::spikyWKernelGrad(position_i -
				//		m_particles.getPosition(neighborIndex));
				//	std::cout << "t->" << t.x << "," << t.y << "," << t.z << std::endl;
				//}
			}
			if (glm::isnan(curl).x || glm::isnan(curl).y || glm::isnan(curl).z)
				continue;
			//curl = glm::normalize(curl);
			real curlLen = glm::length(curl);
			N.x = glm::length(curlX) - curlLen;
			N.y = glm::length(curlY) - curlLen;
			N.z = glm::length(curlZ) - curlLen;
			N = glm::normalize(N);
			if (glm::isnan(N).x || glm::isnan(N).y || glm::isnan(N).z)
				continue;
			glm::vec3 force = 0.000010f * glm::cross(N, curl);
			//glm::vec3 force = 0.00010f * glm::cross(N, curl);

			//glm::vec3 accel = force / static_cast<float>(m_particles.getMass(x));
			//m_particles.setAcceleration(x, m_particles.getAcceleration(x) + force);
			//m_particles.getAcceleration(x) += force;
			//m_particles.setVelocity(x, velocity_i + static_cast<float>(m_timeMgr->getTimeStepSize()) * force);
			deltaVelocity[x] = static_cast<float>(m_timeMgr->getTimeStepSize()) * force;
			//if (test)
			//{
			//	std::cout << "neighbors->" << neighbors.size() << std::endl;
			//	std::cout << "record->" << N.x << "," << N.y << "," << N.z << std::endl;
			//	test = false;
			//}
			//if (record.x < force.x || record.y < force.y || record.z < force.z)
			//	record = force;
		}
		//if (test)
		//{
		//	std::cout << "record->" << record.x << "," << record.y << "," << record.z << std::endl;
		//	test = false;
		//}
		for (unsigned int x = 0; x < nFluidParticles; ++x)
		{
			m_particles.getVelocity(x) += deltaVelocity[x];
		}
	}

	void FluidModelSolver::computeSurfaceTension()
	{
		
	}

	void FluidModelSolver::updateTimeStepSizeCFL(const real & minTimeStep, const real & maxTimeStep)
	{
		//m_timeMgr->setTimeStepSize(0.0083f);
		//return;
		unsigned int nFluidParticles = m_lambda.size();
		const real cflFactor = 1.0;
		real h = m_timeMgr->getTimeStepSize();

		static int count = 100;

		// Approximate max. position change due to current velocities
		real maxVelocity = 0.1;
		const real diameter = static_cast<real>(2.0) * m_particleRadius;
		for (unsigned int x = 0; x < nFluidParticles; ++x)
		{
			const glm::vec3 &velocity = m_particles.getVelocity(x);
			const glm::vec3 &acceleration = m_particles.getAcceleration(x);
			const real velMag = pow(glm::length(velocity + static_cast<float>(h) * acceleration), 2.0);
			if (velMag > maxVelocity)
				maxVelocity = velMag;
		}

		h = cflFactor * 0.40 * (diameter / sqrt(maxVelocity));
		h = std::min(h, maxTimeStep);
		h = std::max(h, minTimeStep);

		m_timeMgr->setTimeStepSize(h);
		if (count > 0)
		{
			//std::cout << "maxVelocity->" << maxVelocity << std::endl;
			//std::cout << "count->" << count << ":" << " cfl:" << cflFactor << ","
			//	<< " diameter:" << diameter << "," << "maxVelocity:" << maxVelocity
			//	<< " ,timeStepSize->" << h << std::endl;
			//std::cout << "maxAcceleration->" << maxAccel << std::endl;
			--count;
		}
	}

	void FluidModelSolver::setParticleRadius(const real & value)
	{
		m_particleRadius = value;
		m_sphRadius = 4.0 * value;
		SPHKernels::setRadius(m_sphRadius);
	}

	void FluidModelSolver::simulate()
	{
		unsigned int nFluidParticles = m_lambda.size();
		const real m_timeStep = m_timeMgr->getTimeStepSize();

		// gravity acceleration.
		clearAccelerations();

		// Update time step size by CFL condition
		updateTimeStepSizeCFL(0.0001, 0.005);
		//updateTimeStepSizeCFL(0.005, 0.005);
		//updateTimeStepSizeCFL(0.0005, 0.007);

		// predict particles' position and velocity.
		parallel_for(blocked_range<size_t>(0, nFluidParticles, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				m_deltaPos[x] = glm::vec3(0.0);
				m_particles.setLastPosition(x, m_particles.getOldPosition(x));
				m_particles.setOldPosition(x, m_particles.getPosition(x));
				PositionBasedFluids::semiImplicitEuler(
					m_timeStep,
					m_particles.getMass(x),
					m_particles.getPosition(x),
					m_particles.getVelocity(x),
					m_particles.getAcceleration(x));
			}
		});
		//for (unsigned int x = 0; x < nFluidParticles; ++x)
		//{
		//	m_deltaPos[x] = glm::vec3(0.0);
		//	m_particles.setLastPosition(x, m_particles.getOldPosition(x));
		//	m_particles.setOldPosition(x, m_particles.getPosition(x));
		//	PositionBasedFluids::semiImplicitEuler(
		//		m_timeStep,
		//		m_particles.getMass(x),
		//		m_particles.getPosition(x),
		//		m_particles.getVelocity(x),
		//		m_particles.getAcceleration(x));
		//}

		// search each particle's neighborhood.
		m_neighborSearch->neighborhoodSearch(m_particles.getPositionGroup(),
			0, nFluidParticles);

		// perform density constraint.
		constraintProjection();

		// update velocities.
		parallel_for(blocked_range<size_t>(0, nFluidParticles, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				PositionBasedFluids::velocityUpdateFirstOrder(
					m_timeStep,
					m_particles.getMass(x),
					m_particles.getPosition(x),
					m_particles.getOldPosition(x),
					m_particles.getVelocity(x)
				);
			}
		});
		//for (unsigned int x = 0; x < nFluidParticles; ++x)
		//{
		//	PositionBasedFluids::velocityUpdateFirstOrder(
		//		m_timeStep,
		//		m_particles.getMass(x),
		//		m_particles.getPosition(x),
		//		m_particles.getOldPosition(x),
		//		m_particles.getVelocity(x)
		//	);
		//}

		// compute viscoity.
		computeXSPHViscosity();
		computeVorticityConfinement();

		// update time stamp.
		m_neighborSearch->updateTimestamp();
		m_timeMgr->setTime(m_timeMgr->getTime() + m_timeStep);
	}

	void FluidModelSolver::resetParticles()
	{
		unsigned int numParticles = m_particles.getNumOfParticles();
		parallel_for(blocked_range<size_t>(0, numParticles, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				glm::vec3 &restPos = m_particles.getRestPosition(x);
				if (x < m_lambda.size())
				{
					m_lambda[x] = 0.0;
					m_density[x] = 0.0;
					m_deltaPos[x] = glm::vec3(0.0f);
				}
				m_particles.setPosition(x, restPos);
				m_particles.setOldPosition(x, restPos);
				m_particles.setLastPosition(x, restPos);
				m_particles.setVelocity(x, glm::vec3(0.0f));
				m_particles.setAcceleration(x, glm::vec3(0.0f));
			}
		}
		);
		//for (unsigned int x = 0; x < numParticles; ++x)
		//{
		//	glm::vec3 &restPos = m_particles.getRestPosition(x);
		//	if (x < m_lambda.size())
		//	{
		//		m_lambda[x] = 0.0;
		//		m_density[x] = 0.0;
		//		m_deltaPos[x] = glm::vec3(0.0f);
		//	}
		//	m_particles.setPosition(x, restPos);
		//	m_particles.setOldPosition(x, restPos);
		//	m_particles.setLastPosition(x, restPos);
		//	m_particles.setVelocity(x, glm::vec3(0.0f));
		//	m_particles.setAcceleration(x, glm::vec3(0.0f));
		//}
	}

	void FluidModelSolver::releaseParticles()
	{
		m_particles.release();
		std::vector<real>().swap(m_lambda);
		std::vector<real>().swap(m_density);
		std::vector<glm::vec3>().swap(m_deltaPos);
	}

	void FluidModelSolver::resizeParticles(const unsigned int & nFluidParticles, 
		const unsigned int &nBoundaryParticles)
	{
		m_particles.resize(nFluidParticles + nBoundaryParticles);
		m_lambda.resize(nFluidParticles);
		m_density.resize(nFluidParticles);
		m_deltaPos.resize(nFluidParticles);
	}
}