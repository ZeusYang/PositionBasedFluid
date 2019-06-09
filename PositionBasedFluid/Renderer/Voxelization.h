#pragma once

#include <GL/glew.h>
#include "Drawable/Drawable.h"

namespace Renderer
{
	class Voxelization
	{
	private:
		GLuint m_cntBuffer;

	public:
		Voxelization() = default;
		~Voxelization() = default;

		bool voxelize(Drawable* target, const float &step, std::vector<glm::vec3> &ret);
	};

}