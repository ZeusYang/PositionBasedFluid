#pragma once

#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>

namespace Simulator
{
	class ParticleGroup
	{
	private:
		std::vector<real> m_masses;
		std::vector<real> m_invMasses;
		std::vector<glm::vec3> m_position;
		std::vector<glm::vec3> m_velocity;
		std::vector<glm::vec3> m_oldPosition;
		std::vector<glm::vec3> m_restPosition;
		std::vector<glm::vec3> m_lastPosition;
		std::vector<glm::vec3> m_acceleration;

	public:
		ParticleGroup() = default;
		~ParticleGroup() { release(); }

		void addParticle(const glm::vec3 &vertex)
		{
			m_masses.push_back(1.0);
			m_invMasses.push_back(1.0);
			m_position.push_back(vertex);
			m_oldPosition.push_back(vertex);
			m_restPosition.push_back(vertex);
			m_lastPosition.push_back(vertex);
			m_velocity.push_back(glm::vec3(0.0f));
			m_acceleration.push_back(glm::vec3(0.0f));
		}

		// Setter.
		void setMass(const unsigned int &index, const real &mass)
		{
			m_masses[index] = mass;
			m_invMasses[index] = (mass != 0) ? (1.0 / mass) : 0.0;
		}
		void setPosition(const unsigned int &index, const glm::vec3 &pos)
		{
			m_position[index] = pos;
		}
		void setLastPosition(const unsigned int &index, const glm::vec3 &pos)
		{
			m_lastPosition[index] = pos;
		}
		void setRestPosition(const unsigned int &index, const glm::vec3 &pos)
		{
			m_restPosition[index] = pos;
		}
		void setOldPosition(const unsigned int &index, const glm::vec3 &pos)
		{
			m_oldPosition[index] = pos;
		}
		void setVelocity(const unsigned int &index, const glm::vec3 &vel)
		{
			m_velocity[index] = vel;
		}
		void setAcceleration(const unsigned int &index, const glm::vec3 &acc)
		{
			m_acceleration[index] = acc;
		}

		// Getter.
		unsigned int getNumOfParticles() const { return m_position.size(); }
		real &getMass(const unsigned int &index) { return m_masses[index]; }
		real &getInvMass(const unsigned int &index) { return m_invMasses[index]; }
		glm::vec3 &getVelocity(const unsigned int &index) { return m_velocity[index]; }
		glm::vec3 &getPosition(const unsigned int &index) { return m_position[index]; }
		glm::vec3 &getOldPosition(const unsigned int &index) { return m_oldPosition[index]; }
		glm::vec3 &getRestPosition(const unsigned int &index) { return m_restPosition[index]; }
		glm::vec3 &getLastPosition(const unsigned int &index) { return m_lastPosition[index]; }
		glm::vec3 &getAcceleration(const unsigned int &index) { return m_acceleration[index]; }
		std::vector<real> &getMassGroup() { return m_masses; }
		std::vector<real> &getInvMassGroup() { return m_invMasses; }
		std::vector<glm::vec3> &getVelocityGroup() { return m_velocity; }
		std::vector<glm::vec3> &getPositionGroup() { return m_position; }
		std::vector<glm::vec3> &getOldPositionGroup() { return m_oldPosition; }
		std::vector<glm::vec3> &getRestPositionGroup() { return m_restPosition; }
		std::vector<glm::vec3> &getLastPositionGroup() { return m_lastPosition; }
		std::vector<glm::vec3> &getAccelerationGroup() { return m_acceleration; }

		// resize, reserve, release.
		void resize(const unsigned int &size)
		{
			m_masses.resize(size);
			m_invMasses.resize(size);
			m_position.resize(size);
			m_velocity.resize(size);
			m_oldPosition.resize(size);
			m_restPosition.resize(size);
			m_lastPosition.resize(size);
			m_acceleration.resize(size);
		}

		void reserve(const unsigned int &size)
		{
			m_masses.reserve(size);
			m_invMasses.reserve(size);
			m_position.reserve(size);
			m_velocity.reserve(size);
			m_oldPosition.reserve(size);
			m_restPosition.reserve(size);
			m_lastPosition.reserve(size);
			m_acceleration.reserve(size);
		}

		void release()
		{
			std::vector<real>().swap(m_masses);
			std::vector<real>().swap(m_invMasses);
			std::vector<glm::vec3>().swap(m_position);
			std::vector<glm::vec3>().swap(m_velocity);
			std::vector<glm::vec3>().swap(m_oldPosition);
			std::vector<glm::vec3>().swap(m_restPosition);
			std::vector<glm::vec3>().swap(m_lastPosition);
			std::vector<glm::vec3>().swap(m_acceleration);
		}
	};
}
