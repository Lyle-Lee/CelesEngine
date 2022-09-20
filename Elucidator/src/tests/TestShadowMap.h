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
	class TestShadowMap : public Test
	{
	public:
		TestShadowMap();
		~TestShadowMap();

		void onUpdate(float deltaTime) override;
		void onRender() override;
		void onImGuiRender() override;
	private:
		std::vector<std::unique_ptr<VertexArray>> m_VAs;
		std::vector<std::unique_ptr<VertexBuffer>> m_VBs;
		std::vector<std::unique_ptr<IndexBuffer>> m_IBs;
		std::unique_ptr<FrameBuffer> m_FB;
		PhongMaterial* m_Material;
		EmissiveMaterial* m_LightMtl;
		std::vector<std::unique_ptr<MeshObject>> m_Objs;
		std::unique_ptr<DirectionalLight> m_Light;
		glm::vec3 m_Translation, m_LightPos, m_CameraPos;
		glm::mat4 m_View, m_Projection;
	};
}