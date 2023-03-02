#include "PCH.h"
#include "CameraController.h"
#include "Celes/Core/Input.h"
#include "Celes/Core/KeyCodes.h"

namespace Celes {

	OrthoCameraController::OrthoCameraController(float aspect, bool rotation)
		: m_AspectRatio(aspect), m_Camera(-aspect * m_ZoomLevel, aspect * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_EnableRotation(rotation)
	{

	}

	void OrthoCameraController::OnUpdate(Timestep ts)
	{
		glm::vec4 move(0.0f);
		if (Input::IsKeyPressed(CE_KEY_A)) move.x -= m_CameraMoveSpeed * ts;
		else if (Input::IsKeyPressed(CE_KEY_D)) move.x += m_CameraMoveSpeed * ts;
		if (Input::IsKeyPressed(CE_KEY_S)) move.y -= m_CameraMoveSpeed * ts;
		else if (Input::IsKeyPressed(CE_KEY_W)) move.y += m_CameraMoveSpeed * ts;
		if (move.x != 0.0f || move.y != 0.0f)
		{
			move = glm::inverse(m_Camera.GetViewMat()) * move;
			m_CameraPos.x += move.x;
			m_CameraPos.y += move.y;

			m_Camera.SetPosition(m_CameraPos);
		}

		if (m_EnableRotation)
		{
			if (Input::IsKeyPressed(CE_KEY_Q)) m_CameraRotation += m_CameraRotateSpeed * ts;
			else if (Input::IsKeyPressed(CE_KEY_E)) m_CameraRotation -= m_CameraRotateSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}
	}

	void OrthoCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(std::bind(&OrthoCameraController::OnMouseScroll, this, std::placeholders::_1));
		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&OrthoCameraController::OnWindowResize, this, std::placeholders::_1));
	}

	bool OrthoCameraController::OnMouseScroll(MouseScrollEvent& e)
	{
		m_ZoomLevel -= e.GetOffsetY() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}

	bool OrthoCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}

}
