#include "Voxelization.h"
#include "Manager/ShaderMgr.h"
#include "Camera/FPSCamera.h"

#include <iostream>

namespace Renderer
{
	bool Voxelization::voxelize(Drawable* target, const float & step, std::vector<glm::vec3>& ret)
	{
		// shader
		ShaderMgr::ptr shaderMgr = ShaderMgr::getSingleton();
		unsigned int voxelizeCount = shaderMgr->loadShader("voxelizeCount",
			"./glsl/voxelizeCount.vert", "./glsl/voxelizeCount.frag");
		Shader::ptr shader = shaderMgr->getShader(voxelizeCount);

		// bounding box and resolution.
		glm::vec3 min, max;
		glm::ivec3 resolution;
		target->getAABB(min, max);

		glm::vec3 range(max.x - min.x, max.y - min.y, max.z - min.z);
		resolution.x = static_cast<int>(range.x / step);
		resolution.y = static_cast<int>(range.y / step);
		resolution.z = static_cast<int>(range.z / step);
		int length = static_cast<int>(resolution.x * resolution.y * resolution.z);

		// camera
		glm::vec3 cameraPos;
		cameraPos.x = (min.x + max.x) * 0.5f;
		cameraPos.y = (min.y + max.y) * 0.5f;
		cameraPos.z = max.z + 0.2f;
		FPSCamera::ptr camera(new FPSCamera(cameraPos));
		camera->lookAt(glm::vec3(0.0f, 0.0f, -1.0f));
		camera->setOrthographicProject(-range.x * 0.51, +range.x * 0.51, -range.y * 0.51, +range.y * 0.51, 0.1, range.z * 1.2f + 0.2f);


		// polygon mode.
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// generate ssbo.
		glGenBuffers(1, &m_cntBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cntBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, length * sizeof(int), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_cntBuffer);
		
		// bind shader and ssbo.
		shader->bind();
		shader->setVec3("boxMin", min);
		shader->setFloat("step", step);
		shader->setVec3("resolution", resolution);
		int *writePtr = reinterpret_cast<int*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
		for (int x = 0; x < length; ++x)
		{
			writePtr[x] = 0;
		}
		if (!glUnmapBuffer(GL_SHADER_STORAGE_BUFFER))
			std::cout << "unMap error\n" << std::endl;

		// draw and count.
		target->render(camera, nullptr, nullptr, shader);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// get count buffer.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cntBuffer);

		int *readPtr = reinterpret_cast<int*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

		if (readPtr != nullptr)
		{
			for (int x = 0; x < length; ++x)
			{
				if (*(readPtr + x) != 0)
				{
					int iy = x / (resolution.x * resolution.z);
					int iz = (x - iy * resolution.x * resolution.z) / (resolution.x);
					int ix = x - iy * resolution.x * resolution.z - iz * resolution.x;
					ret.push_back(min + glm::vec3(ix * step, iy * step, iz * step));
				}
			}
		}
		else
		{
			std::cout << "nullptr error!\n";
		}

		glUnmapBuffer(m_cntBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDeleteBuffers(1, &m_cntBuffer);

		return false;
	}

}