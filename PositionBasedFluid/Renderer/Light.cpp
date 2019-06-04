#include "Light.h"

namespace Renderer
{
	void Light::setLightColor(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
	{
		m_ambient = amb;
		m_diffuse = diff;
		m_specular = spec;
	}
	void DirectionalLight::setDirection(glm::vec3 dir)
	{
		m_direction = glm::normalize(dir);
	}

	void DirectionalLight::setLightUniform(Shader::ptr shader, Camera3D::ptr camera)
	{
		shader->setVec3("cameraPos", camera->getPosition());
		shader->setVec3("dirLight.direction", m_direction);
		shader->setVec3("dirLight.ambient", m_ambient);
		shader->setVec3("dirLight.diffuse", m_diffuse);
		shader->setVec3("dirLight.specular", m_specular);
	}
	glm::vec3 DirectionalLight::getDirection() const
	{
		return m_direction;
	}
}
