#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Renderer
{
	struct Vertex
	{
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
		glm::vec3 color;

		Vertex() = default;

		Vertex(float px, float py, float pz, float nx, float ny,
			float nz, float u, float v, float r, float g, float b)
		{
			position = glm::vec3(px, py, pz);
			normal = glm::vec3(nx, ny, nz);
			texcoord = glm::vec2(u, v);
			color = glm::vec3(r, g, b);
		}
	};

	class Mesh
	{
	private:
		unsigned int m_vao, m_vbo, m_ebo;
		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;

	public:
		typedef std::shared_ptr<Mesh> ptr;

		Mesh() = default;

		Mesh(const std::vector<Vertex> &vert,
			const std::vector<unsigned int> &indi);

		virtual ~Mesh();

		unsigned int getVertexArrayObject() const
		{
			return m_vao;
		}

		unsigned int getVertexBufferObject() const
		{
			return m_vbo;
		}

		unsigned int getIndexBufferObject() const
		{
			return m_ebo;
		}

		void draw(bool instance, int amount = 0) const;

	protected:
		void setupMesh(const std::vector<Vertex> &vert,
			const std::vector<unsigned int> &indi);
		void clearMesh();
	};

}

