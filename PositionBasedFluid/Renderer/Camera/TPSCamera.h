#pragma once

#include "Camera3D.h"
#include "Transform3D.h"

namespace Renderer
{
	class TPSCamera : public Camera3D
	{
	private:
		mutable bool m_dirty;
		glm::vec3 m_cameraPos;
		Transform3D m_target;
		glm::vec2 m_distanceLimt;
		double m_yaw, m_pitch, m_distance;
		float m_mouseSty, m_moveSpeed, m_wheelSty;

	public:
		typedef std::shared_ptr<TPSCamera> ptr;

		// ctor/dtor.
		TPSCamera(glm::vec3 target, float yaw, float pitch, float dist);
		virtual ~TPSCamera() = default;

		// Getter.
		glm::mat4 getTargetMatrix();
		virtual glm::vec3 getPosition();
		virtual glm::mat4 getViewMatrix();
		virtual glm::mat4 getInvViewMatrix();

		// Setter.
		void setYaw(float yaw) { m_yaw = yaw; }
		void setPitch(float pitch) { m_pitch = pitch; }
		void setDistance(float dist) { m_distance = dist; }
		void setMoveSpeed(float speed) { m_moveSpeed = speed; }
		void setMouseSensitivity(float sens) { m_mouseSty = sens; }
		void setWheelSensitivity(float sens) { m_wheelSty = sens; }
		void setDistanceLimt(float min, float max) { m_distanceLimt = glm::vec2(min, max); }

		// Key/Mouse reaction.
		virtual void onKeyPress(float deltaTime, char key);
		virtual void onWheelMove(double delta);
		virtual void onMouseMove(double deltaX, double deltaY, std::string button);

	private:
		void update();
	};

}

