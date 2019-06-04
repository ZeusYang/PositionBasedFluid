#pragma once

#include <vector>
#include <memory>

#include "Geometry.h"
#include "../Light.h"
#include "../Camera/Camera3D.h"
#include "../Camera/Transform3D.h"

namespace Renderer
{
	class Drawable
	{
	protected:
		bool m_instance = false;
		int m_instanceNum = 0;
		Transform3D m_transformation;

		unsigned int m_shaderIndex;
		std::vector<unsigned int> m_texIndex;
		std::vector<unsigned int> m_meshIndex;

	public:
		typedef std::shared_ptr<Drawable> ptr;

		Drawable() = default;
		virtual ~Drawable() = default;

		virtual void render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera) = 0;
		virtual void renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera) = 0;

		void addMesh(unsigned int meshIndex, unsigned int texIndex)
		{
			m_texIndex.push_back(texIndex);
			m_meshIndex.push_back(meshIndex);
		}

		Transform3D *getTransformation()
		{
			return &m_transformation;
		}

	protected:
		void renderImp();
	};

	class DrawableList : public Drawable
	{
	private:
		std::vector<Drawable::ptr> m_list;

	public:
		typedef std::shared_ptr<DrawableList> ptr;

		DrawableList() = default;
		virtual ~DrawableList() = default;

		unsigned int addDrawable(Drawable *object)
		{
			Drawable::ptr drawable(object);
			m_list.push_back(drawable);
			return m_list.size() - 1;
		}

		virtual void render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera)
		{
			for (auto &it : m_list)
				it->render(camera, sunLight, lightCamera);
		}

		virtual void renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera)
		{
			for (auto &it : m_list)
				it->renderDepth(shader, lightCamera);
		}

	};

	class SkyDome : public Drawable
	{
	public:
		typedef std::shared_ptr<SkyDome> ptr;

		SkyDome(unsigned int shaderIndex)
		{
			m_shaderIndex = shaderIndex;
		}

		SkyDome() = default;

		virtual void render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera);
		virtual void renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera) {}
	};

	class SimpleObject : public Drawable
	{
	public:
		
		SimpleObject(unsigned int shaderIndex)
		{
			m_shaderIndex = shaderIndex;
		}
		
		~SimpleObject() = default;

		virtual void render(Camera3D::ptr camera, Light::ptr sunLight, Camera3D::ptr lightCamera);
		virtual void renderDepth(Shader::ptr shader, Camera3D::ptr lightCamera);
	};
}