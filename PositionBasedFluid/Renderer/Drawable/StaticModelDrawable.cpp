#include "StaticModelDrawable.h"

#include "../Manager/MeshMgr.h"
#include "../Manager/ShaderMgr.h"
#include "../Manager/TextureMgr.h"

#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Renderer
{
	StaticModelDrawable::StaticModelDrawable(unsigned int shaderIndex, const std::string &path)
	{
		m_shaderIndex = shaderIndex;
		loadModel(path);
	}

	StaticModelDrawable::~StaticModelDrawable()
	{
	}

	void StaticModelDrawable::render(Camera3D::ptr camera, Light::ptr sunLight,
		Camera3D::ptr lightCamera, Shader::ptr shader)
	{
		// render the model.
		if (!m_visiable) return;
		if (shader == nullptr)
		{
			shader = ShaderMgr::getSingleton()->getShader(m_shaderIndex);
			shader->bind();
		}
		if(sunLight)
			sunLight->setLightUniform(shader, camera);
		shader->setInt("image", 0);
		// depth map.
		Texture::ptr depthMap = TextureMgr::getSingleton()->getTexture("shadowDepth");
		if (depthMap != nullptr)
		{
			shader->setInt("depthMap", 1);
			depthMap->bind(1);
		}
		if (lightCamera != nullptr)
			shader->setMat4("lightSpaceMatrix",
				lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		else
			shader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));
		// object matrix.
		shader->setBool("instance", false);
		shader->setBool("receiveShadow", m_receiveShadow);
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		shader->setMat4("viewMatrix", camera->getViewMatrix());
		shader->setMat4("projectMatrix", camera->getProjectMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void StaticModelDrawable::renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
	{
		shader->bind();
		shader->setBool("instance", false);
		shader->setMat4("lightSpaceMatrix",
			lightCamera->getProjectMatrix() * lightCamera->getViewMatrix());
		shader->setMat4("modelMatrix", m_transformation.getWorldMatrix());
		this->renderImp();
		ShaderMgr::getSingleton()->unBindShader();
	}

	void StaticModelDrawable::loadModel(const std::string &path)
	{
		// load the model file.
		m_min = glm::vec3(+FLT_MAX);
		m_max = glm::vec3(-FLT_MAX);
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate 
			| aiProcess_FlipUVs | aiProcess_GenNormals);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		m_directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}

	void StaticModelDrawable::processNode(aiNode *node, const aiScene *scene)
	{
		// process all meshes.
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			unsigned int meshIndex, texIndex = 1000000;
			processMesh(mesh, scene, meshIndex, texIndex);
			this->addMesh(meshIndex);
			if(texIndex != 1000000)
				this->addTexture(texIndex);
		}
		// process children' nodes.
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void StaticModelDrawable::processMesh(aiMesh * mesh, const aiScene * scene,
		unsigned int & meshIndex, unsigned int & texIndex)
	{
		// process mesh.
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		for (unsigned int x = 0; x < mesh->mNumVertices;++ x)
		{
			Vertex vertex;
			// position.
			vertex.position = glm::vec3(
				mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z);
			// normal.
			vertex.normal = glm::vec3(
				mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z);
			// texcoord.
			if (mesh->mTextureCoords[0])
				vertex.texcoord = glm::vec2(
					mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y);
			else
				vertex.texcoord = glm::vec2(0.0f, 0.0f);
			vertex.color = vertex.normal;
			vertices.push_back(vertex);
			// bounding box.
			if (mesh->mVertices[x].x < m_min.x)
				m_min.x = mesh->mVertices[x].x;
			if (mesh->mVertices[x].y < m_min.y)
				m_min.y = mesh->mVertices[x].y;
			if (mesh->mVertices[x].z < m_min.z)
				m_min.z = mesh->mVertices[x].z;
			if (mesh->mVertices[x].x > m_max.x)
				m_max.x = mesh->mVertices[x].x;
			if (mesh->mVertices[x].y > m_max.y)
				m_max.y = mesh->mVertices[x].y;
			if (mesh->mVertices[x].z > m_max.z)
				m_max.z = mesh->mVertices[x].z;
			//m_min.x = std::min(m_min.x, mesh->mVertices[x].x);
			//m_min.y = std::min(m_min.y, mesh->mVertices[x].y);
			//m_min.z = std::min(m_min.z, mesh->mVertices[x].z);
			//m_max.x = std::max(m_max.x, mesh->mVertices[x].x);
			//m_max.y = std::max(m_max.y, mesh->mVertices[x].y);
			//m_max.z = std::max(m_max.z, mesh->mVertices[x].z);
		}

		for (unsigned int x = 0; x < mesh->mNumFaces;++ x)
		{
			aiFace face = mesh->mFaces[x];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int y = 0; y < face.mNumIndices;++ y)
				indices.push_back(face.mIndices[y]);
		}

		Mesh *target = new Mesh(vertices, indices);
		meshIndex = MeshMgr::getSingleton()->loadMesh(target);

		// process material.
		TextureMgr::ptr textureMgr = TextureMgr::getSingleton();
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			aiString nameStr;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &nameStr);
			std::string name(nameStr.C_Str());
			texIndex = textureMgr->loadTexture2D(name, m_directory + "/" + name);
		}
	}

}