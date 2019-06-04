#pragma once

#include "Utils.h"
#include "HashMap.h"
#include "ParticleGroup.h"

#include <glm/glm.hpp>
#include <iostream>

namespace Simulator
{
	template<>
	inline unsigned int hashFunction<glm::ivec3*>(glm::ivec3* const &key)
	{
		const int p1 = 73856093 * key->x;
		const int p2 = 19349663 * key->y;
		const int p3 = 83492791 * key->z;
		return p1 + p2 + p3;
	}
	
	class NeighborSearch
	{
	public:
		struct SpatialCell
		{
			unsigned long m_timeStamp;
			std::vector<unsigned int> m_particleIndices;
		};

	private:
		real m_radius;
		real m_cellGridSize;
		unsigned int m_maxNeighbors;
		unsigned int m_numParticles;
		unsigned int m_currentTimestamp;
		unsigned int m_maxParticlesPerCell;
		HashMap<glm::ivec3*, SpatialCell*> m_spatialMap;
		std::vector<std::vector<unsigned int>>  m_neighbors;

	public:
		NeighborSearch(unsigned int numParticles = 0, real radius = 0.1, unsigned int maxNeihbors = 60,
			unsigned int maxParticlesPercell = 50);

		~NeighborSearch();

		// Neighborhood search using spatial hashing.
		void neighborhoodSearch(const std::vector<glm::vec3> &particles,
			const unsigned int &start, const unsigned int &num);

		// Setter.
		void cleanup();
		void updateTimestamp() { ++ m_currentTimestamp; }
		void setRadius(const real &radi) { m_cellGridSize = radi; m_radius = radi; }
		void setMaxNeighbors(const unsigned int &num) { m_maxNeighbors = num; }

		// Getter.
		real getRadius()const { return m_radius; }
		unsigned int getMaxNeighbors()const { return m_maxNeighbors; }
		std::vector<unsigned int> &getNeighbors(const unsigned int &index) { return m_neighbors[index]; }
		unsigned int getNeighborsNum(const unsigned int &index) const { return m_neighbors[index].size(); }
		unsigned int getNeighborIndex(const unsigned int &i, const unsigned int &j) const { return m_neighbors[i][j]; }

	private:
		static int floor(const real &v)
		{
			return static_cast<int>(v + 32768.f) - 32768;
		}

	};
}