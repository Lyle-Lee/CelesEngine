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

namespace test
{
	class TestMeshObject : public Test
	{
	public:
		TestMeshObject();
		~TestMeshObject();

		void onUpdate(float deltaTime) override;
		void onRender() override;
		void onImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		Material* m_Material;
		std::unique_ptr<MeshObject> m_Obj;
		glm::vec3 m_Translation, m_LightPos, m_CameraPos;
		glm::mat4 m_View, m_Projection;
	};
}