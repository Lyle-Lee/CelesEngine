#pragma once

#include "Celes/Renderer/Camera.h"
#include "Celes/Core/Core.h"

namespace Celes {

	class CE_API SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetProjectionType(ProjectionType type) { m_ProjType = type; CalcProjection(); }
		void SetViewportSize(uint32_t width, uint32_t height);

		void SetOrthoInfo(float size, float nearClip, float farClip);
		void SetOrthoSize(float size) { m_OrthoSize = size; CalcProjection(); }
		void SetOrthoNearClip(float nearClip) { m_OrthoNear = nearClip; CalcProjection(); }
		void SetOrthoFarClip(float farClip) { m_OrthoFar = farClip; CalcProjection(); }

		void SetPerspectiveInfo(float fov, float nearClip, float farClip);
		void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; CalcProjection(); }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; CalcProjection(); }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; CalcProjection(); }

		inline ProjectionType GetProjectionType() const { return m_ProjType; }

		inline float GetOrthoSize() const { return m_OrthoSize; }
		inline float GetOrthoNearClip() const { return m_OrthoNear; }
		inline float GetOrthoFarClip() const { return m_OrthoFar; }

		inline float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		inline float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		inline float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
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
