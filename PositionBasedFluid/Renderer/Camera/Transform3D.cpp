#include "Transform3D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Renderer
{
	const glm::vec3 Transform3D::LocalUp(0.0f, 1.0f, 0.0f);
	const glm::vec3 Transform3D::LocalForward(0.0f, 0.0f, 1.0f);
	const glm::vec3 Transform3D::LocalRight(1.0f, 0.0f, 0.0f);

	Transform3D::Transform3D()
		:m_dirty(true), m_scale(1.0, 1.0, 1.0), m_translation(0.0, 0.0, 0.0)
	{
		m_worldMatrix = glm::mat4(1.0f);
		m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}

	glm::mat4 Transform3D::getWorldMatrix()
	{
		if (m_dirty)
		{
			m_dirty = false;
			m_worldMatrix = glm::mat4(1.0f);
			m_worldMatrix = glm::translate(m_worldMatrix, m_translation);
			m_worldMatrix = m_worldMatrix * glm::mat4_cast(m_rotation);
			m_worldMatrix = glm::scale(m_worldMatrix, m_scale);
		}
		return m_worldMatrix;
	}

	glm::mat4 Transform3D::getInvWorldMatrix()
	{
		return glm::mat4();
	}

	void Transform3D::scale(const glm::vec3 & ds)
	{
		m_dirty = true;
		m_scale.x *= ds.x;
		m_scale.y *= ds.y;
		m_scale.z *= ds.z;
	}

	void Transform3D::translate(const glm::vec3 & dt)
	{
		m_dirty = true;
		m_translation += dt;
	}

	void Transform3D::rotate(const glm::vec3 & axis, float angle)
	{
		m_dirty = true;
		m_rotation = glm::angleAxis(glm::radians(angle), axis) * m_rotation;
	}

	void Transform3D::setScale(const glm::vec3 & s)
	{
		m_dirty = true;
		m_scale = s;
	}

	void Transform3D::setRotation(const glm::quat & r)
	{
		m_dirty = true;
		m_rotation = r;
	}

	void Transform3D::setTranslation(const glm::vec3 & t)
	{
		m_dirty = true;
		m_translation = t;
	}

	glm::vec3 Transform3D::forward() const
	{
		return m_rotation * LocalForward;
	}

	glm::vec3 Transform3D::up() const
	{
		return m_rotation * LocalUp;
	}

	glm::vec3 Transform3D::right() const
	{
		return m_rotation * LocalRight;
	}

}