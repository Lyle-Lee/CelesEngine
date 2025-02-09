#include "PCH.h"
#include "Light.h"

namespace Celes {

	DirectionalLight::DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, bool hasShadowMap, const glm::vec3& radiance)
		: m_Position(pos), m_Direction(glm::normalize(dir)), m_Radiance(radiance)
	{
		UpdateViewMat();

		if (hasShadowMap)
		{
			FrameBufferDescription fbDesc;
			fbDesc.Width = 2048;
			fbDesc.Height = 2048;
			fbDesc.AttachmentDesc = { TextureFormat::DEPTH16 };
			m_FBO = FrameBuffer::Create(fbDesc);
		}
	}

	void DirectionalLight::UpdateViewMat()
	{
		glm::vec3 upDir = glm::cross(m_Direction, glm::cross(m_Direction, glm::normalize(-m_Position)));
		m_ViewMat = glm::lookAtRH(m_Position, m_Position - m_Direction, upDir);
		m_VP = m_ProjectionMat * m_ViewMat;
	}

}
