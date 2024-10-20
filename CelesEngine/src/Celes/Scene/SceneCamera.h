#pragma once

#include "Celes/Renderer/Camera.h"
#include "Celes/Core/Core.h"

namespace Celes {

	class SceneCamera : public Camera
	{
	public:
		enum class CE_API ProjectionType { Perspective = 0, Orthographic = 1 };

		CE_API SceneCamera();
		CE_API virtual ~SceneCamera() = default;

		CE_API void SetProjectionType(ProjectionType type) { m_ProjType = type; CalcProjection(); }
		CE_API void SetViewportSize(uint32_t width, uint32_t height);

		CE_API void SetOrthoInfo(float size, float nearClip, float farClip);
		CE_API void SetOrthoSize(float size) { m_OrthoSize = size; CalcProjection(); }
		CE_API void SetOrthoNearClip(float nearClip) { m_OrthoNear = nearClip; CalcProjection(); }
		CE_API void SetOrthoFarClip(float farClip) { m_OrthoFar = farClip; CalcProjection(); }

		CE_API void SetPerspectiveInfo(float fov, float nearClip, float farClip);
		CE_API void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; CalcProjection(); }
		CE_API void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; CalcProjection(); }
		CE_API void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; CalcProjection(); }

		CE_API inline ProjectionType GetProjectionType() const { return m_ProjType; }

		CE_API inline float GetOrthoSize() const { return m_OrthoSize; }
		CE_API inline float GetOrthoNearClip() const { return m_OrthoNear; }
		CE_API inline float GetOrthoFarClip() const { return m_OrthoFar; }

		CE_API inline float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		CE_API inline float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		CE_API inline float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
	private:
		void CalcProjection();

		ProjectionType m_ProjType = ProjectionType::Orthographic;

		float m_OrthoSize = 10.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

		float m_ViewportAspectRatio = 1.0f;
	};

}
