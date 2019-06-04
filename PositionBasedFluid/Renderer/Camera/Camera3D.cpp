#include "Camera3D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Renderer
{
	const glm::vec3 Camera3D::LocalForward(0.0f, 0.0f, -1.0f);
	const glm::vec3 Camera3D::LocalUp(0.0f, 1.0f, 0.0f);
	const glm::vec3 Camera3D::LocalRight(1.0f, 0.0f, 0.0f);

	void Camera3D::changeAspect(float aspect)
	{
		m_aspect = aspect;
		m_projectMatrix = glm::perspective(glm::radians(m_angle), m_aspect, m_near, m_far);
	}

	void Camera3D::setPerspectiveProject(float angle, float aspect, float near, float far)
	{
		m_angle = angle;
		m_aspect = aspect;
		m_near = near;
		m_far = far;
		m_projectMatrix = glm::perspective(glm::radians(angle), aspect, near, far);
	}

	void Camera3D::setOrthographicProject(float left, float right, float bottom,
		float top, float near, float far)
	{
		m_projectMatrix = glm::ortho(left, right, bottom, top, near, far);
	}

	glm::mat4 Camera3D::getProjectMatrix()
	{
		return m_projectMatrix;
	}

}