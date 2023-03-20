#pragma once

#include "Celes/Renderer/Camera.h"
#include "Celes/Core/Core.h"

namespace Celes {

	class CE_API SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthoInfo(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);
		void SetOrthoSize(float size) { m_OrthoSize = size; CalcProjection(); }

		inline float GetOrthoSize() const { return m_OrthoSize; }
	private:
		void CalcProjection();

		float m_OrthoSize = 10.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;
		float m_ViewportAspectRatio = 1.0f;
	};

}
