#pragma once

#include <glm.hpp>
#include "Celes/Core/Core.h"

namespace Celes {

	class CE_API Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection): m_Projection(projection) {}
		virtual ~Camera() = default;

		inline const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

	class CE_API OrthoCamera
	{
	public:
		OrthoCamera(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& pos) { m_Position = pos; UpdateViewMat(); }
		void SetRotation(float rotation) { m_Rotation = rotation; UpdateViewMat(); }
		void SetProjection(float left, float right, float bottom, float top);

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const float GetRotation() const { return m_Rotation; }

		inline const glm::mat4& GetProjectionMat() const { return m_ProjectionMat; }
		inline const glm::mat4& GetViewMat() const { return m_ViewMat; }
		const glm::mat4& GetVP() const { return m_VP; }
	private:
		void UpdateViewMat();

		glm::mat4 m_ProjectionMat, m_ViewMat, m_VP;

		glm::vec3 m_Position;
		float m_Rotation = 0.0f;
	};

	class CE_API PerspectiveCamera
	{
	public:
		PerspectiveCamera(float fov, float aspect, float zNear, float zFar);

		void SetPosition(const glm::vec3& pos);
		void SetFocus(const glm::vec3& focusPoint) { m_LookAt = focusPoint; UpdateViewMat(); }
		void SetViewDistance(float dist);

		inline const glm::vec3& GetPosition() const { return m_Position; }

		inline const glm::mat4& GetProjectionMat() const { return m_ProjectionMat; }
		inline const glm::mat4& GetViewMat() const { return m_ViewMat; }
		const glm::mat4& GetVP() const { return m_VP; }
	private:
		void UpdateViewMat();

		glm::mat4 m_ProjectionMat, m_ViewMat, m_VP;

		glm::vec3 m_Position, m_LookAt, m_UpDir;
		float m_ViewDistance;
	};

}
