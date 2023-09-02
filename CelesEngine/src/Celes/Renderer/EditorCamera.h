#pragma once

#include "Camera.h"
#include "Celes/Core/Timestep.h"
#include "Celes/Events/Event.h"
#include "Celes/Events/MouseEvent.h"
#include <glm.hpp>

namespace Celes {

	class CE_API EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep dTime);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetViewMat() const { return m_ViewMat; }
		// Get the result of projection matrix dot view matrix
		glm::mat4 GetVP() const { return m_Projection * m_ViewMat; }

		glm::vec3 GetUpDir() const;
		glm::vec3 GetRightDir() const;
		glm::vec3 GetForwardDir() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		inline float GetPitch() const { return m_Pitch; }
		inline float GetYaw() const { return m_Yaw; }
	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrollEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const { return 0.8f; }
		float ZoomSpeed() const;

		glm::vec3 CalculatePosition();

		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMat = glm::mat4(1.0f);
		glm::vec3 m_Position = { 0.0f, 0.0f, -10.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePos = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;
	};

}
