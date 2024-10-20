#pragma once

#include "Camera.h"
#include "Celes/Core/Timestep.h"
#include "Celes/Events/ApplicationEvent.h"
#include "Celes/Events/MouseEvent.h"

namespace Celes {

	struct OrthoCameraBounds
	{
		float Left, Right, Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthoCameraController
	{
	public:
		CE_API OrthoCameraController(float aspect, bool rotation = false);

		CE_API void OnUpdate(Timestep ts);
		CE_API void OnEvent(Event& e);

		CE_API inline OrthoCamera& GetCamera() { return m_Camera; }
		CE_API inline const OrthoCamera& GetCamera() const { return m_Camera; }

		CE_API inline const OrthoCameraBounds& GetBounds() const { return m_Bounds; }
		CE_API void ResizeBounds(float width, float height);

		CE_API void SetZoomLevel(float level) { m_ZoomLevel = level; FitZoom(); }
	private:
		bool OnMouseScroll(MouseScrollEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void FitZoom();

		float m_AspectRatio, m_ZoomLevel = 1.0f;
		OrthoCameraBounds m_Bounds;
		OrthoCamera m_Camera;
		
		glm::vec3 m_CameraPos = glm::vec3(0.0f);
		float m_CameraMoveSpeed = 1.0f;

		bool m_EnableRotation;
		float m_CameraRotation = 0.0f, m_CameraRotateSpeed = 60.0f;
	};

}
