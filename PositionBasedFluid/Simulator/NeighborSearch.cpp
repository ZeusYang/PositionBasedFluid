#include "NeighborSearch.h"

#include <tbb/parallel_for.h>

using namespace tbb;

namespace Simulator
{
	NeighborSearch::NeighborSearch(
		unsigned int numParticles, 
		real radius,
		unsigned int maxNeihbors,
		unsigned int maxParticlesPercell)
		: m_spatialMap(numParticles * 2)
	{
		m_cellGridSize = radius;
		m_radius = radius;
		m_numParticles = numParticles;
		if (numParticles != 0)
			m_neighbors.resize(numParticles, std::vector<unsigned int>());
		m_currentTimestamp = 0;
	}

	NeighborSearch::~NeighborSearch()
	{
		cleanup();
	}

	void NeighborSearch::neighborhoodSearch(const std::vector<glm::vec3>& particles,
		const unsigned int &start, const unsigned int &num)
	{
		const real factor = 1.0 / m_cellGridSize;
		unsigned int numParticles = particles.size();
		m_neighbors.assign(numParticles, std::vector<unsigned int>());
		// hash mapping.
		for (unsigned int x = 0; x < numParticles; ++x)
		{
			glm::ivec3 cellPos;
			cellPos.x = floor(particles[x].x * factor) + 1;
			cellPos.y = floor(particles[x].y * factor) + 1;
			cellPos.z = floor(particles[x].z * factor) + 1;
			SpatialCell *&entry = m_spatialMap[&cellPos];

			if (entry != nullptr)
			{
				if (entry->m_timeStamp != m_currentTimestamp)
				{
					entry->m_timeStamp = m_currentTimestamp;
					entry->m_particleIndices.clear();
				}
			}
			else
			{
				SpatialCell *newEntry = new SpatialCell();
				newEntry->m_particleIndices.reserve(m_maxParticlesPerCell);
				newEntry->m_timeStamp = m_currentTimestamp;
				entry = newEntry;
			}
			entry->m_particleIndices.push_back(x);
		}

		// hash lookup.

		unsigned int end = start + num;
		parallel_for(blocked_range<size_t>(start, end, 5000),
			[&](blocked_range<size_t> &r)
		{
			for (size_t x = r.begin(); x != r.end(); ++x)
			{
				const int cellPos1 = floor(particles[x].x * factor);
				const int cellPos2 = floor(particles[x].y * factor);
				const int cellPos3 = floor(particles[x].z * factor);
				for (unsigned char i = 0; i < 3; ++i)
				{
					for (unsigned char j = 0; j < 3; ++j)
					{
						for (unsigned char k = 0; k < 3; ++k)
						{
							glm::ivec3 cellPos(cellPos1 + i, cellPos2 + j, cellPos3 + k);
							SpatialCell * const *entry = m_spatialMap.query(&cellPos);
							if ((entry != nullptr) && (*entry != nullptr) &&
								((*entry)->m_timeStamp == m_currentTimestamp))
							{
								for (unsigned int m = 0; m < (*entry)->m_particleIndices.size(); ++m)
								{
									const unsigned int index = (*entry)->m_particleIndices[m];
									// not consider itself.
									if (index != x)
									{
										const real dist = glm::length(particles[x] - particles[index]);
										if (dist < m_radius)
										{
											m_neighbors[x].push_back(index);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		);

		//for (unsigned int x = start; x < end; ++x)
		//{
		//	const int cellPos1 = floor(particles[x].x * factor);
		//	const int cellPos2 = floor(particles[x].y * factor);
		//	const int cellPos3 = floor(particles[x].z * factor);
		//	for (unsigned char i = 0; i < 3; ++i)
		//	{
		//		for (unsigned char j = 0; j < 3; ++j)
		//		{
		//			for (unsigned char k = 0; k < 3; ++k)
		//			{
		//				glm::ivec3 cellPos(cellPos1 + i, cellPos2 + j, cellPos3 + k);
		//				SpatialCell * const *entry = m_spatialMap.query(&cellPos);
		//				if ((entry != nullptr) && (*entry != nullptr) && ((*entry)->m_timeStamp == m_currentTimestamp))
		//				{
		//					for (unsigned int m = 0; m < (*entry)->m_particleIndices.size(); ++m)
		//					{
		//						const unsigned int index = (*entry)->m_particleIndices[m];
		//						// not consider itself.
		//						if (index != x)
		//						{
		//							const real dist = glm::length(particles[x] - particles[index]);
		//							if (dist < m_radius)
		//							{
		//								m_neighbors[x].push_back(index);
		//							}
		//						}
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
	}

	void NeighborSearch::cleanup()
	{
		std::vector<std::vector<unsigned int>>().swap(m_neighbors);
		m_numParticles = 0;
		for (unsigned int x = 0; x < m_spatialMap.bucketCount(); ++x)
		{
			HashMap<glm::ivec3*, SpatialCell*>::KeyValueMap *map = m_spatialMap.getKeyValueMap(x);
			if (map)
			{
				for (HashMap<glm::ivec3*, SpatialCell*>::KeyValueMap::iterator it = map->begin();
					it != map->end(); ++it)
				{
					SpatialCell *entry = it->second;
					delete entry;
					it->second = nullptr;
				}
			}
		}
	}
}