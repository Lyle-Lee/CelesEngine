#pragma once

#include "Camera.h"
#include "Celes/Core/Timestep.h"
#include "Celes/Events/ApplicationEvent.h"
#include "Celes/Events/MouseEvent.h"

namespace Celes {

	class CE_API OrthoCameraController
	{
	public:
		OrthoCameraController(float aspect, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		inline OrthoCamera& GetCamera() { return m_Camera; }
		inline const OrthoCamera& GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScroll(MouseScrollEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		float m_AspectRatio, m_ZoomLevel = 1.0f;
		OrthoCamera m_Camera;
		
		glm::vec3 m_CameraPos = glm::vec3(0.0f);
		float m_CameraMoveSpeed = 1.0f;

		bool m_EnableRotation;
		float m_CameraRotation = 0.0f, m_CameraRotateSpeed = 60.0f;
	};

}
