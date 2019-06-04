#include "Drawable.h"

#include "../Manager/MeshMgr.h"
#include "../Manager/TextureMgr.h"
#include "../Manager/ShaderMgr.h"

#include <iostream>

namespace Renderer
{
	void Drawable::renderImp()
	{
		// render each mesh.
		MeshMgr::ptr meshMgr = MeshMgr::getSingleton();
		TextureMgr::ptr textureMgr = TextureMgr::getSingleton();
		for (int x = 0; x < m_meshIndex.size(); ++x)
		{
			if (x < m_texIndex.size())
				textureMgr->bindTexture(m_texIndex[x], 0);
			else
				textureMgr->unBindTexture(m_texIndex[x]);
			meshMgr->drawMesh(m_meshIndex[x], m_instance, m_instanceNum);
		}
	}

	void SkyDome::render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera)
	{
		Shader::ptr shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
		shader->bind();
		shader->setInt("image", 0);
		shader->setMat4("viewMatrix", glm::mat4(glm::mat3(camera->getViewMatrix())));
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void SimpleObject::render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera)
	{
		Shader::ptr shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
		shader->bind();
		if (sunLight)
			sunLight->setLightUniform(shader, camera);
		shader->setInt("image", 0);
		// depth map.
		Texture::ptr depthMap = TextureMgr::getSingleton()->getTexture("shadowDepth");
		if (depthMap != nullptr)
		{
			shader->setInt("depthMap", 1);
			depthMap->bind(1);
		}
		// light space matrix.
		if (lightCamera != nullptr)
			shader->setMat4("lightSpaceMatrix",
				lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		else
			shader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
		// object matrix.
		shader->setBool("instance", false);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void SimpleObject::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		shader->bind();
		shader->setBool("instance", false);
		shader->setMat4("lightSpaceMatrix", 
			lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}
}