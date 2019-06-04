#include "TPSCamera.h"

#include <iostream>

namespace Renderer
{
	TPSCamera::TPSCamera(glm::vec3 target, float yaw, float pitch, float dist)
		:m_yaw(yaw), m_pitch(pitch), m_distance(dist)
	{
		m_dirty = true;
		m_mouseSty = 0.05f;
		m_moveSpeed = 2.5f;
		m_wheelSty = 0.1f;
		m_distanceLimt = glm::vec2(1.0f, 50.0f);
		m_target.setTranslation(target);
	}

	glm::mat4 TPSCamera::getTargetMatrix()
	{
		return m_target.getWorldMatrix();
	}

	glm::vec3 TPSCamera::getPosition()
	{
		update();
		return m_cameraPos;
	}

	glm::mat4 TPSCamera::getViewMatrix()
	{
		update();
		return m_viewMatrix;
	}

	glm::mat4 TPSCamera::getInvViewMatrix()
	{
		update();
		return glm::inverse(m_viewMatrix);
	}

	void TPSCamera::onKeyPress(float deltaTime, char key)
	{
		// nothing here.
	}

	void TPSCamera::onWheelMove(double delta)
	{
		m_dirty = true;
		m_distance += m_wheelSty * (-delta);
		if (m_distance > m_distanceLimt.y) m_distance = m_distanceLimt.y;
		if (m_distance < m_distanceLimt.x) m_distance = m_distanceLimt.x;
	}

	void TPSCamera::onMouseMove(double deltaX, double deltaY, std::string button)
	{
		if (button == "RIGHT")
		{
			m_dirty = true;
			m_pitch += m_mouseSty * (-deltaY);
			if (m_pitch < 0.0)m_pitch = 0.0;
			if (m_pitch > 89.9)m_pitch = 89.9;
		}
		else if (button == "LEFT")
		{
			m_dirty = true;
			m_yaw += m_mouseSty * (-deltaX);
			m_yaw = fmod(m_yaw, 360.0f);
		}
	}

	void TPSCamera::update()
	{
		if (m_dirty)
		{
			m_dirty = false;
			glm::vec3 target = m_target.translation();
			float height = m_distance * sin(glm::radians(m_pitch));
			float horizon = m_distance * cos(glm::radians(m_pitch));
			m_cameraPos.y = target.y + height;
			m_cameraPos.x = target.x + horizon * sin(glm::radians(m_yaw));
			m_cameraPos.z = target.z + horizon * cos(glm::radians(m_yaw));
			m_viewMatrix = glm::lookAt(m_cameraPos, target, LocalUp);
		}
	}

}