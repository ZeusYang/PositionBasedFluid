#include "Voxelization.h"
#include "Manager/ShaderMgr.h"
#include "Camera/FPSCamera.h"

#include <iostream>
#include <algorithm>

namespace Renderer
{
	bool Voxelization::voxelize(Drawable* target, const float & step, std::vector<glm::vec3>& ret)
	{
		ret.clear();
		// shader
		ShaderMgr::ptr shaderMgr = ShaderMgr::getSingleton();
		unsigned int voxelizeCount = shaderMgr->loadShader("voxelizeCount",
			"./glsl/voxelizeCount.vert", "./glsl/voxelizeCount.frag", "./glsl/voxelizeCount.geom");
		Shader::ptr shader = shaderMgr->getShader(voxelizeCount);

		// bounding box and resolution.
		glm::vec3 min, max;
		glm::ivec3 resolution;
		target->getAABB(min, max);

		glm::vec3 range(max.x - min.x, max.y - min.y, max.z - min.z);
		resolution.x = static_cast<int>(range.x / step) + 1;
		resolution.y = static_cast<int>(range.y / step) + 1;
		resolution.z = static_cast<int>(range.z / step) + 1;
		int length = static_cast<int>((resolution.x) * (resolution.y) * (resolution.z));

		// Cameras
		float offset = 0.2f;
		glm::vec3 cameraPosZ, cameraPosX, cameraPosY;
		// looking along z axis.
		cameraPosZ.x = (min.x + max.x) * 0.5f;
		cameraPosZ.y = (min.y + max.y) * 0.5f;
		cameraPosZ.z = max.z + offset;
		FPSCamera::ptr cameraZ(new FPSCamera(cameraPosZ));
		cameraZ->lookAt(glm::vec3(0.0f, 0.0f, -1.0f), Camera3D::LocalUp);
		cameraZ->setOrthographicProject(-range.x * 0.51, +range.x * 0.51, -range.y * 0.51,
			+range.y * 0.51, 0.1, range.z * 1.2f + offset);
		// looking along x axis.
		cameraPosX.x = max.x + offset;
		cameraPosX.y = (min.y + max.y) * 0.5f;
		cameraPosX.z = (min.z + max.z) * 0.5f;
		FPSCamera::ptr cameraX(new FPSCamera(cameraPosX));
		cameraX->lookAt(glm::vec3(-1.0f, 0.0f, 0.0f), Camera3D::LocalUp);
		cameraX->setOrthographicProject(-range.z * 0.51, +range.z * 0.51, -range.y * 0.51,
			+range.y * 0.51, 0.1, range.x * 1.2f + offset);
		// looking along y axis.
		cameraPosY.x = (min.x + max.x) * 0.5f;
		cameraPosY.y = max.y + offset;
		cameraPosY.z = (min.z + max.z) * 0.5f;
		FPSCamera::ptr cameraY(new FPSCamera(cameraPosY));
		cameraY->lookAt(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0, 1.0, 0.001));
		cameraY->setOrthographicProject(-range.x * 0.51, +range.x * 0.51, -range.z * 0.51,
			+range.z * 0.51, 0.1, range.y * 1.2f + offset);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_MULTISAMPLE);
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
		shader->setVec2("halfPixel[0]", glm::vec2(1.0f / resolution.z, 1.0f / resolution.y));
		shader->setVec2("halfPixel[1]", glm::vec2(1.0f / resolution.x, 1.0f / resolution.z));
		shader->setVec2("halfPixel[2]", glm::vec2(1.0f / resolution.x, 1.0f / resolution.y));
		shader->setMat4("viewProject[0]", cameraX->getProjectMatrix() * cameraX->getViewMatrix());
		shader->setMat4("viewProject[1]", cameraY->getProjectMatrix() * cameraY->getViewMatrix());
		shader->setMat4("viewProject[2]", cameraZ->getProjectMatrix() * cameraZ->getViewMatrix());
		shader->setMat4("viewProjectInverse[0]", glm::inverse(cameraX->getProjectMatrix() * cameraX->getViewMatrix()));
		shader->setMat4("viewProjectInverse[1]", glm::inverse(cameraY->getProjectMatrix() * cameraY->getViewMatrix()));
		shader->setMat4("viewProjectInverse[2]", glm::inverse(cameraZ->getProjectMatrix() * cameraZ->getViewMatrix()));

		int *writePtr = reinterpret_cast<int*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
		for (int x = 0; x < length; ++x)
		{
			writePtr[x] = 0;
		}
		if (!glUnmapBuffer(GL_SHADER_STORAGE_BUFFER))
			std::cout << "unMap error\n" << std::endl;

		// draw and count.
		target->render(cameraY, nullptr, nullptr, shader);
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
			std::cout << "nullptr error!\n";

		glUnmapBuffer(m_cntBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDeleteBuffers(1, &m_cntBuffer);

		return false;
	}

}
