#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Renderer
{
	class Transform3D
	{
	private:
		mutable bool m_dirty;

		glm::vec3 m_scale;
		glm::quat m_rotation;
		glm::vec3 m_translation;
		glm::mat4 m_worldMatrix;

	public:
		// Object's local axis.
		static const glm::vec3 LocalForward;
		static const glm::vec3 LocalUp;
		static const glm::vec3 LocalRight;

		// ctor/dtor
		Transform3D();
		~Transform3D() = default;

		// Getter.
		glm::mat4 getWorldMatrix();
		glm::mat4 getInvWorldMatrix();

		// Transformation.
		void scale(const glm::vec3 &ds);
		void translate(const glm::vec3 &dt);
		void rotate(const glm::vec3 &axis, float angle);
		void setScale(const glm::vec3 &s);
		void setRotation(const glm::quat &r);
		void setTranslation(const glm::vec3 &t);

		// Query object's axis.
		glm::vec3 forward() const;
		glm::vec3 up() const;
		glm::vec3 right() const;

		// Getter.
		glm::vec3 scale() const { return m_scale; }
		glm::quat rotation() const { return m_rotation; }
		glm::vec3 translation() const { return m_translation; }
	};

}

