#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Renderer
{
	class Camera3D
	{
	protected:
		// view matrix and projection matrix.
		glm::mat4 m_viewMatrix = glm::mat4(1.0f);
		glm::mat4 m_projectMatrix = glm::mat4(1.0f);
		float m_angle, m_aspect, m_near, m_far;

	public:
		typedef std::shared_ptr<Camera3D> ptr;

		// Local axis.
		static const glm::vec3 LocalForward;
		static const glm::vec3 LocalUp;
		static const glm::vec3 LocalRight;

		// ctor/dtor.
		Camera3D() = default;
		virtual ~Camera3D() = default;

		// Setter.
		void changeAspect(float aspect);
		void setPerspectiveProject(float angle, float aspect, float near, float far);
		void setOrthographicProject(float left, float right, float bottom, float top,
			float near, float far);

		// Getter.
		glm::mat4 getProjectMatrix();
		virtual glm::vec3 getPosition() = 0;
		virtual glm::mat4 getViewMatrix() = 0;
		virtual glm::mat4 getInvViewMatrix() = 0;

		// Key/Mouse reaction.
		virtual void onKeyPress(float deltaTime, char key) = 0;
		virtual void onWheelMove(double delta) = 0;
		virtual void onMouseMove(double deltaX, double deltaY, std::string button) = 0;
	};
}