#include "PCH.h"
#include "SceneCamera.h"
#include <gtc/matrix_transform.hpp>

namespace Celes {

	SceneCamera::SceneCamera()
	{
		CalcProjection();
	}

	void SceneCamera::SetOrthoInfo(float size, float nearClip, float farClip)
	{
		m_ProjType = ProjectionType::Orthographic;
		m_OrthoSize = size;
		m_OrthoNear = nearClip;
		m_OrthoFar = farClip;
		CalcProjection();
	}

	void SceneCamera::SetPerspectiveInfo(float fov, float nearClip, float farClip)
	{
		m_ProjType = ProjectionType::Perspective;
		m_PerspectiveFOV = fov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		CalcProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportAspectRatio = (float)width / (float)height;
		CalcProjection();
	}

	void SceneCamera::CalcProjection()
	{
		if (m_ProjType == ProjectionType::Orthographic)
		{
			float bottom = -m_OrthoSize * 0.5f;
			float top = -bottom;
			float left = m_ViewportAspectRatio * bottom;
			float right = -left;
			m_Projection = glm::ortho(left, right, bottom, top, m_OrthoNear, m_OrthoFar);
		}
		else
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_ViewportAspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
	}

}
