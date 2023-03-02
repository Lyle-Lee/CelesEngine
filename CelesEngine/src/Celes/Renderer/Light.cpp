#include "PCH.h"
#include "Light.h"

namespace Celes {

	DirectionalLight::DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, bool hasShadowMap, const glm::vec3& radiance)
		: m_Position(pos), m_Direction(glm::normalize(dir)), m_Radiance(radiance)
	{
		UpdateViewMat();

		if (hasShadowMap)
		{
			m_FBO.reset(FrameBuffer::Create(2048, 2048));
			m_ShadowMap = Texture2D::Create(m_FBO->GetWidth(), m_FBO->GetHeight());
			m_ShadowMap->Bind();
			m_FBO->AddAttachment(m_ShadowMap);
			m_FBO->SetRenderBuffer();
			m_FBO->UnBind();
		}
	}

	void DirectionalLight::UpdateViewMat()
	{
		m_ViewMat = glm::lookAtRH(m_Position, m_Position - m_Direction, glm::vec3(1.0f, 0.0f, 0.0f));
		m_VP = m_ProjectionMat * m_ViewMat;
	}

}
