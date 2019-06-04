#pragma once

#include <glm/glm.hpp>

#include "Camera/Camera3D.h"
#include "Manager/Shader.h"

namespace Renderer
{
	class Light
	{
	protected:
		glm::vec3 m_ambient;
		glm::vec3 m_diffuse;
		glm::vec3 m_specular;

	public:
		typedef std::shared_ptr<Light> ptr;

		Light() = default;
		~Light() = default;

		virtual void setLightColor(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);
		virtual void setLightUniform(Shader::ptr shader, Camera3D::ptr camera) = 0;
	};

	class DirectionalLight : public Light
	{
	private:
		glm::vec3 m_direction;

	public:
		typedef std::shared_ptr<DirectionalLight> ptr;

		DirectionalLight() = default;
		virtual ~DirectionalLight() = default;

		void setDirection(glm::vec3 dir);
		virtual void setLightUniform(Shader::ptr shader, Camera3D::ptr camera);

		glm::vec3 getDirection() const;

	};

}
