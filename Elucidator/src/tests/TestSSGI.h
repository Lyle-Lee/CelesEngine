#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <memory>

#include "Test.h"
#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../MeshObject.h"
#include "../Light.h"
#include "../FrameBuffer.h"

namespace test
{
	class TestSSGI : public Test
	{
	public:
		TestSSGI();
		~TestSSGI();

		void onUpdate(float deltaTime) override;
		void onRender() override;
		void onImGuiRender() override;
	private:
		std::vector<std::unique_ptr<MeshObject>> m_Objs;
		std::vector<std::unique_ptr<Texture>> m_Textures;

		std::vector<std::unique_ptr<VertexArray>> m_VAs;
		std::vector<std::unique_ptr<VertexBuffer>> m_VBs;
		std::vector<std::unique_ptr<IndexBuffer>> m_IBs;

		std::unique_ptr<FrameBuffer> m_LightFB, m_GBuffer;
		std::vector<std::unique_ptr<Texture>> m_GBufferTextures;
		std::vector<unsigned int> m_Attachments;
		unsigned int m_AttachmentCnt = 4; // Kd, Normal, Visibility, Depth

		Material* m_Material;
		EmissiveMaterial* m_LightMtl;
		Material* m_GBufferMtl;
		std::unique_ptr<Shader> m_ComputeShader;
		
		std::unique_ptr<DirectionalLight> m_Light;
		glm::vec3 m_Translation, m_LightPos, m_CameraPos;
		glm::mat4 m_View, m_Projection;
	};
}