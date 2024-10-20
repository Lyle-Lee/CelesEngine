#pragma once

#include <glm.hpp>
#include "Celes/Core/Core.h"

namespace Celes {

	class Camera
	{
	public:
		CE_API Camera() = default;
		CE_API Camera(const glm::mat4& projection) : m_Projection(projection) {}
		CE_API virtual ~Camera() = default;

		CE_API inline const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

	class OrthoCamera
	{
	public:
		CE_API OrthoCamera(float left, float right, float bottom, float top);

		CE_API void SetPosition(const glm::vec3& pos) { m_Position = pos; UpdateViewMat(); }
		CE_API void SetRotation(float rotation) { m_Rotation = rotation; UpdateViewMat(); }
		CE_API void SetProjection(float left, float right, float bottom, float top);

		CE_API inline const glm::vec3& GetPosition() const { return m_Position; }
		CE_API inline const float GetRotation() const { return m_Rotation; }

		CE_API inline const glm::mat4& GetProjectionMat() const { return m_ProjectionMat; }
		CE_API inline const glm::mat4& GetViewMat() const { return m_ViewMat; }
		CE_API const glm::mat4& GetVP() const { return m_VP; }
	private:
		void UpdateViewMat();

		glm::mat4 m_ProjectionMat, m_ViewMat, m_VP;

		glm::vec3 m_Position;
		float m_Rotation = 0.0f;
	};

	class PerspectiveCamera
	{
	public:
		CE_API PerspectiveCamera(float fov, float aspect, float zNear, float zFar);

		CE_API void SetPosition(const glm::vec3& pos);
		CE_API void SetFocus(const glm::vec3& focusPoint) { m_LookAt = focusPoint; UpdateViewMat(); }
		CE_API void SetViewDistance(float dist);

		CE_API inline const glm::vec3& GetPosition() const { return m_Position; }

		CE_API inline const glm::mat4& GetProjectionMat() const { return m_ProjectionMat; }
		CE_API inline const glm::mat4& GetViewMat() const { return m_ViewMat; }
		CE_API const glm::mat4& GetVP() const { return m_VP; }
	private:
		void UpdateViewMat();

		glm::mat4 m_ProjectionMat, m_ViewMat, m_VP;

		glm::vec3 m_Position, m_LookAt, m_UpDir;
		float m_ViewDistance;
	};

}
