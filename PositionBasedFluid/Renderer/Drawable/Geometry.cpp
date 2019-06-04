#include "Geometry.h"

#include <glm/glm.hpp>

#define M_PI 3.1415926535
#define M_2PI 6.2831853071

namespace Renderer
{
	Triangle::Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Vertex current;
		current.color = glm::vec3(1.0, 0.0, 0.0);
		current.normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
		// v1
		current.position = v1;
		current.texcoord = glm::vec2(0.5f, 1.0f);
		vertices.push_back(current);
		indices.push_back(0);
		// v2
		current.position = v2;
		current.texcoord = glm::vec2(0.0f, 0.0f);
		vertices.push_back(current);
		indices.push_back(1);
		// v3
		current.position = v3;
		current.texcoord = glm::vec2(1.0f, 0.0f);
		vertices.push_back(current);
		indices.push_back(2);

		setupMesh(vertices, indices);
	}

	Cube::Cube(float width, float height, float depth)
	{
		float halfW = width * 0.5f;
		float halfH = height * 0.5f;
		float halfD = depth * 0.5f;

		std::vector<Vertex>  vertices =
		{
			Vertex(-halfW, -halfH, -halfD, 0.f, 0.f, -1.f, 0.f, 1.f, 1.0, 0.0, 0.0),
			Vertex(-halfW, +halfH, -halfD, 0.f, 0.f, -1.f, 0.f, 0.f, 0.0, 0.0, 1.0),
			Vertex(+halfW, +halfH, -halfD, 0.f, 0.f, -1.f, 1.f, 0.f, 0.0, 1.0, 0.0),
			Vertex(+halfW, -halfH, -halfD, 0.f, 0.f, -1.f, 1.f, 1.f, 1.0, 0.0, 1.0),

			Vertex(-halfW, -halfH, +halfD, -1.f, 0.f, 0.f, 0.f, 1.f, 1.0, 0.0, 0.0),
			Vertex(-halfW, +halfH, +halfD, -1.f, 0.f, 0.f, 0.f, 0.f, 0.0, 0.0, 1.0),
			Vertex(-halfW, +halfH, -halfD, -1.f, 0.f, 0.f, 1.f, 0.f, 0.0, 1.0, 1.0),
			Vertex(-halfW, -halfH, -halfD, -1.f, 0.f, 0.f, 1.f, 1.f, 0.0, 1.0, 0.0),

			Vertex(+halfW, -halfH, +halfD, 0.f, 0.f, 1.f, 0.f, 1.f, 1.0, 0.0, 0.0),
			Vertex(+halfW, +halfH, +halfD, 0.f, 0.f, 1.f, 0.f, 0.f, 0.0, 1.0, 0.0),
			Vertex(-halfW, +halfH, +halfD, 0.f, 0.f, 1.f, 1.f, 0.f, 0.0, 0.0, 1.0),
			Vertex(-halfW, -halfH, +halfD, 0.f, 0.f, 1.f, 1.f, 1.f, 1.0, 1.0, 0.0),

			Vertex(+halfW, -halfH, -halfD, 1.f, 0.f, 0.f, 0.f, 1.f, 1.0, 0.0, 0.0),
			Vertex(+halfW, +halfH, -halfD, 1.f, 0.f, 0.f, 0.f, 0.f, 1.0, 0.0, 0.0),
			Vertex(+halfW, +halfH, +halfD, 1.f, 0.f, 0.f, 1.f, 0.f, 1.0, 0.0, 0.0),
			Vertex(+halfW, -halfH, +halfD, 1.f, 0.f, 0.f, 1.f, 1.f, 1.0, 0.0, 0.0),

			Vertex(-halfW, +halfH, -halfD, 0.f, 1.f, 0.f, 0.f, 1.f, 1.0, 0.0, 0.0),
			Vertex(-halfW, +halfH, +halfD, 0.f, 1.f, 0.f, 0.f, 0.f, 0.0, 0.0, 1.0),
			Vertex(+halfW, +halfH, +halfD, 0.f, 1.f, 0.f, 1.f, 0.f, 1.0, 0.0, 0.0),
			Vertex(+halfW, +halfH, -halfD, 0.f, 1.f, 0.f, 1.f, 1.f, 0.0, 1.0, 1.0),

			Vertex(-halfW, -halfH, +halfD, 0.f, -1.f, 0.f, 0.f, 1.f, 1.0, 0.0, 0.0),
			Vertex(-halfW, -halfH, -halfD, 0.f, -1.f, 0.f, 0.f, 0.f, 0.0, 1.0, 0.0),
			Vertex(+halfW, -halfH, -halfD, 0.f, -1.f, 0.f, 1.f, 0.f, 0.0, 0.0, 1.0),
			Vertex(+halfW, -halfH, +halfD, 0.f, -1.f, 0.f, 1.f, 1.f, 1.0, 0.0, 1.0)
		};

		std::vector<unsigned int> indices =
		{
			//! front.
			 0, 1, 2,
			 0, 2, 3,

			 //! back.
			 4, 5, 6,
			 4, 6, 7,

			 //! left.
			 8, 9,10,
			 8,10,11,

			 //! right.
			 12,13,14,
			 12,14,15,

			 //! up.
			 16,17,18,
			 16,18,19,

			 //! down.
			 20,21,22,
			 20,22,23
		};

		setupMesh(vertices, indices);
	}

	Sphere::Sphere(float radius, int numRings, int numSegments)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		float fDeltaRingAngle = static_cast<float>(M_PI / numRings);
		float fDeltaSegAngle = static_cast<float>(M_2PI / numSegments);
		unsigned short wVerticeIndex = 0;

		//! Generate the group of rings for the sphere.
		for (int ring = 0; ring <= numRings; ring++) 
		{
			float r0 = static_cast<float>(radius) * sinf(ring * fDeltaRingAngle);
			float y0 = static_cast<float>(radius) * cosf(ring * fDeltaRingAngle);

			//! Generate the group of segments for the current ring.
			for (int seg = 0; seg <= numSegments; seg++) 
			{
				float x0 = r0 * sinf(seg * fDeltaSegAngle);
				float z0 = r0 * cosf(seg * fDeltaSegAngle);

				glm::vec3 vNormal = glm::normalize(glm::vec3(x0, y0, z0));

				vertices.push_back(
					Vertex(x0, y0, z0,
						vNormal.x, vNormal.y, vNormal.z,
						static_cast<float>(seg) / static_cast<float>(numSegments),
						static_cast<float>(ring) / static_cast<float>(numRings),
						vNormal.x, vNormal.y, vNormal.z));
				if (ring != numRings) 
				{
					//! each vertex (except the last) has six indicies pointing to it.
					indices.push_back(static_cast<unsigned short>(wVerticeIndex + numSegments + 1));
					indices.push_back(static_cast<unsigned short>(wVerticeIndex));
					indices.push_back(static_cast<unsigned short>(wVerticeIndex + numSegments));
					indices.push_back(static_cast<unsigned short>(wVerticeIndex + numSegments + 1));
					indices.push_back(wVerticeIndex + 1);
					indices.push_back(wVerticeIndex);
					wVerticeIndex++;
				}
			}; //! end for seg.
		} //! end for ring.
		
		setupMesh(vertices, indices);
	}

	Plane::Plane(float width, float height)
	{
		float half_width = width / 2.0f;
		float half_height = height / 2.0f;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Vertex current;
		current.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		float texcoordScale = width / 8.0f;
		// v0
		current.position = glm::vec3(-half_width, 0.0f, -half_height);
		current.texcoord = glm::vec2(0.0f, texcoordScale);
		current.color = glm::vec3(1.0f, 0.0f, 0.0f);
		vertices.push_back(current);
		// v1
		current.position = glm::vec3(-half_width, 0.0f, +half_height);
		current.texcoord = glm::vec2(0.0f, 0.0f);
		current.color = glm::vec3(0.0f, 1.0f, 0.0f);
		vertices.push_back(current);
		// v2
		current.position = glm::vec3(+half_width, 0.0f, +half_height);
		current.texcoord = glm::vec2(texcoordScale, 0.0f);
		current.color = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices.push_back(current);
		// v3
		current.position = glm::vec3(+half_width, 0.0f, -half_height);
		current.texcoord = glm::vec2(texcoordScale, texcoordScale);
		current.color = glm::vec3(1.0f, 0.0f, 1.0f);
		vertices.push_back(current);
		indices = { 0,1,2,0,2,3 };

		setupMesh(vertices, indices);
	}
}
