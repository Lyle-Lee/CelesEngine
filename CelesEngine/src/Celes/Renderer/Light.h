#pragma once

#include "Buffer.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Celes {

	/*class Light
	{
	public:
		~Light() = default;

		virtual glm::mat4& GetLightVP() const = 0;
		virtual glm::vec3& GetRadiance() const = 0;
		virtual Ref<FrameBuffer> GetFrameBuffer() const = 0;
	};*/

	class DirectionalLight
	{
	public:
		CE_API DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, bool hasShadowMap = false, const glm::vec3& radiance = glm::vec3(1.0f));
		CE_API ~DirectionalLight() {}

		CE_API void SetLightPos(const glm::vec3& pos) { m_Position = pos; UpdateViewMat(); }
		CE_API void SetLightDir(const glm::vec3& dir) { m_Direction = dir; UpdateViewMat(); }
		CE_API void SetRadiance(const glm::vec3& radiance) { m_Radiance = radiance; }

		CE_API inline const glm::mat4& GetLightVP() const { return m_VP; }
		CE_API inline const glm::vec3& GetLightPos() const { return m_Position; }
		CE_API inline const glm::vec3& GetLightDir() const { return m_Direction; }
		CE_API inline const glm::vec3& GetRadiance() const { return m_Radiance; }
		CE_API inline const Ref<FrameBuffer>& GetFrameBuffer() const { return m_FBO; }
		CE_API inline const Ref<Texture2D>& GetShadowMap() const { return m_FBO->GetAttachmentTexture(); }
	private:
		void UpdateViewMat();

		glm::mat4 m_ProjectionMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1e-2f, 132.0f);
		glm::mat4 m_ViewMat, m_VP;
		glm::vec3 m_Position, m_Direction, m_Radiance;
		Ref<FrameBuffer> m_FBO = nullptr;
		//Ref<Texture2D> m_ShadowMap = nullptr;
	};

}
