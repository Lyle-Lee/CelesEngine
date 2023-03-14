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

	class CE_API OrthoCameraController
	{
	public:
		OrthoCameraController(float aspect, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		inline OrthoCamera& GetCamera() { return m_Camera; }
		inline const OrthoCamera& GetCamera() const { return m_Camera; }

		inline const OrthoCameraBounds& GetBounds() const { return m_Bounds; }
		void ResizeBounds(float width, float height);

		void SetZoomLevel(float level) { m_ZoomLevel = level; FitZoom(); }
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
