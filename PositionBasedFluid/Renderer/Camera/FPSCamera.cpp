#include "FPSCamera.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

namespace Renderer
{

	FPSCamera::FPSCamera(glm::vec3 pos)
		:m_dirty(true), m_translation(pos)
	{
		m_mouseSty = 0.05f;
		m_moveSpeed = 2.5f;
		m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}

	glm::mat4 FPSCamera::getViewMatrix()
	{
		if (m_dirty)
		{
			m_dirty = false;
			m_viewMatrix = glm::mat4(1.0f);
			m_viewMatrix = glm::mat4_cast(glm::conjugate(m_rotation));
			m_viewMatrix = glm::translate(m_viewMatrix, -m_translation);
		}
		return m_viewMatrix;
	}

	glm::mat4 FPSCamera::getInvViewMatrix()
	{
		if (m_dirty)
		{
			m_dirty = false;
			m_viewMatrix = glm::mat4_cast(glm::conjugate(m_rotation));
			m_viewMatrix = glm::translate(m_viewMatrix, -m_translation);
		}
		return glm::inverse(m_viewMatrix);
	}

	void FPSCamera::onKeyPress(float deltaTime, char key)
	{
		switch (key)
		{
		case 'W':
			this->translate(forward() * deltaTime * m_moveSpeed);
			break;
		case 'S':
			this->translate(-forward() * deltaTime * m_moveSpeed);
			break;
		case 'A':
			this->translate(-right() * deltaTime * m_moveSpeed);
			break;
		case 'D':
			this->translate(+right() * deltaTime * m_moveSpeed);
			break;
		case 'Q':
			this->translate(up() * deltaTime * m_moveSpeed);
			break;
		case 'E':
			this->translate(-up() * deltaTime * m_moveSpeed);
			break;
		default:
			break;
		}
	}

	void FPSCamera::onWheelMove(double delta)
	{
		// nothing now.
	}

	void FPSCamera::onMouseMove(double deltaX, double deltaY, std::string button)
	{
		this->rotate(LocalUp, -deltaX * m_mouseSty);
		this->rotate(right(), deltaY * m_mouseSty);
	}

	void FPSCamera::lookAt(glm::vec3 dir, glm::vec3 up)
	{
		m_dirty = true;
		m_rotation = glm::quatLookAt(dir, up);
	}

	void FPSCamera::translate(const glm::vec3 & dt)
	{
		m_dirty = true;
		m_translation += dt;
	}

	void FPSCamera::rotate(const glm::vec3 & axis, float angle)
	{
		m_dirty = true;
		m_rotation = glm::angleAxis(glm::radians(angle), axis) * m_rotation;
	}

	void FPSCamera::setTranslation(const glm::vec3 & t)
	{
		m_dirty = true;
		m_translation = t;
	}

	void FPSCamera::setRotation(const glm::quat & r)
	{
		m_dirty = true;
		m_rotation = r;
	}

	glm::vec3 FPSCamera::forward() const
	{
		return m_rotation * LocalForward;
	}

	glm::vec3 FPSCamera::up() const
	{
		return m_rotation * LocalUp;
	}

	glm::vec3 FPSCamera::right() const
	{
		return m_rotation * LocalRight;
	}

}
