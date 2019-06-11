#pragma once

#include "Camera3D.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Renderer
{
	class FPSCamera : public Camera3D
	{
	private:
		mutable bool m_dirty;       // Should update or not.
		glm::vec3 m_translation;     // Camera's translation.
		glm::quat m_rotation;       // Camera's rotation.
		float m_mouseSty, m_moveSpeed;

	public:
		typedef std::shared_ptr<FPSCamera> ptr;

		FPSCamera(glm::vec3 pos);
		virtual ~FPSCamera() = default;

		virtual glm::vec3 getPosition()
		{
			return m_translation;
		}

		// Getter.
		virtual glm::mat4 getViewMatrix();
		virtual glm::mat4 getInvViewMatrix();

		// Setter.
		void setMouseSensitivity(float sens)
		{
			m_mouseSty = sens;
		}

		void setMoveSpeed(float speed)
		{
			m_moveSpeed = speed;
		}

		// Key/Mouse reaction.
		virtual void onKeyPress(float deltaTime, char key);
		virtual void onWheelMove(double delta);
		virtual void onMouseMove(double deltaX, double deltaY, std::string button);

		// Transform camera's axis.
		void lookAt(glm::vec3 dir, glm::vec3 up);
		void translate(const glm::vec3 &dt);
		void rotate(const glm::vec3 &axis, float angle);
		void setTranslation(const glm::vec3 &t);
		void setRotation(const glm::quat &r);

		// Query for camera's axis.
		glm::vec3 forward() const;
		glm::vec3 up() const;
		glm::vec3 right() const;
	};
}

