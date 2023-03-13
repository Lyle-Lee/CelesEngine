#include "PCH.h"
#include "Camera.h"
#include <gtc/matrix_transform.hpp>

namespace Celes {

	OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
		: m_ProjectionMat(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMat(1.0f), m_Position(0.0f)
	{
		m_VP = m_ProjectionMat * m_ViewMat;
	}

	void OrthoCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMat = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_VP = m_ProjectionMat * m_ViewMat;
	}

	void OrthoCamera::UpdateViewMat()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
							  glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		m_ViewMat = glm::inverse(transform);
		m_VP = m_ProjectionMat * m_ViewMat;
	}

	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float zNear, float zFar)
		: m_ProjectionMat(glm::perspective(fov, aspect, zNear, zFar)), m_Position(0.0f), m_LookAt(0.0f), m_UpDir(0.0f, 1.0f, 0.0f), m_ViewDistance(2.0f)
	{
		UpdateViewMat();
	}

	void PerspectiveCamera::SetPosition(const glm::vec3& pos)
	{
		m_Position = m_LookAt - glm::normalize(m_LookAt - pos) * m_ViewDistance;
		UpdateViewMat();
	}

	void PerspectiveCamera::SetViewDistance(float dist)
	{
		m_ViewDistance = dist;
		m_Position = m_LookAt - glm::normalize(m_LookAt - m_Position) * m_ViewDistance;
		UpdateViewMat();
	}

	void PerspectiveCamera::UpdateViewMat()
	{
		m_ViewMat = glm::lookAtRH(m_Position, m_LookAt, m_UpDir);
		m_VP = m_ProjectionMat * m_ViewMat;
	}

}
