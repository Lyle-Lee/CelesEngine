#include <Celes.h>
#include <Celes/Core/EntryPoint.h>
#include <ImGui/imgui.h>
#include <gtc/matrix_transform.hpp>
#include <Platform/OpenGL/OpenGLShader.h>
#include "Sandbox2D.h"

class ExampleLayer : public Celes::Layer
{
public:
	ExampleLayer() : Layer("Example"), m_Camera(glm::pi<float>() / 2.0f, 16.0f / 9.0f, 1.0f, -5.0f), m_CameraPos(0.0f, 0.0f, 1.5f), m_ObjPos(0.0f)
	{
		m_VertexArray = Celes::VertexArray::Create();
		m_QuadVA = Celes::VertexArray::Create();

		m_Lights.push_back(std::make_shared<Celes::DirectionalLight>(glm::vec3(0.0f, 10.0f, 5.0f), glm::vec3(0.0f, 10.0f, 5.0f), true));

		// Cube + Quad
		float vertices[8 * 8] = {
			-0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // pos, normal, texCoord
			-0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, 1.0f, 0.0f
		};
		float quadVertices[8 * 4] = {
			-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f 
		};
		Celes::Ref<Celes::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Celes::VertexBuffer::Create(vertices, sizeof(vertices)));

		vertexBuffer->SetLayout({
			{"aPosition", Celes::ShaderDataType::Float3},
			{"aNormal", Celes::ShaderDataType::Float3},
			{"aTexCoord", Celes::ShaderDataType::Float2}
			});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Cube + Quad
		uint32_t indices[6 * 6] = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			4, 0, 1, 1, 5, 4,
			7, 3, 2, 2, 6, 7,
			5, 1, 2, 2, 6, 5,
			7, 3, 0, 0, 4, 7
		};
		uint32_t quadIndices[6] = {
			0, 1, 2, 2, 3, 0
		};
		Celes::Ref<Celes::IndexBuffer> indexBuffer;
		indexBuffer.reset(Celes::IndexBuffer::Create(indices, sizeof(indices)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Quad
		vertexBuffer.reset(Celes::VertexBuffer::Create(quadVertices, sizeof(quadVertices)));
		vertexBuffer->SetLayout({
			{"aPosition", Celes::ShaderDataType::Float3},
			{"aNormal", Celes::ShaderDataType::Float3},
			{"aTexCoord", Celes::ShaderDataType::Float2}
			});
		m_QuadVA->AddVertexBuffer(vertexBuffer);
		indexBuffer.reset(Celes::IndexBuffer::Create(quadIndices, sizeof(quadIndices)));
		m_QuadVA->SetIndexBuffer(indexBuffer);

		auto shader = m_ShaderLib.Load("assets/shaders/PhongVertexShader.glsl", "assets/shaders/PhongFragmentShader.glsl");

		//m_Texture = Celes::Texture2D::Create("assets/textures/checkerbox.png");
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->Bind();
		//std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformInt("uTexture", 0);
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uKa", glm::vec3(0.0f, 0.5f, 1.0f));
		//std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uKd", glm::vec3(0.796f, 0.52f, 0.482f));
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uKs", glm::vec3(0.5f, 0.5f, 0.5f));
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uLightIntensity", glm::vec3(225.0f, 225.0f, 225.0f));

		m_ShaderLib.Load("assets/shaders/ShadowVertexShader.glsl", "assets/shaders/ShadowFragmentShader.glsl");
	}

	void OnUpdate(Celes::Timestep dTime) override
	{
		//CE_TRACE("Delta time: {0}s ({1}ms)", dTime.GetSeconds(), dTime.GetMilliseconds());

		// For 3D camera
		if (Celes::Input::IsKeyPressed(CE_KEY_A)) m_CameraViewDist += m_CameraMoveSpeed * dTime;
		else if (Celes::Input::IsKeyPressed(CE_KEY_D)) m_CameraViewDist -= m_CameraMoveSpeed * dTime;

		if (Celes::Input::IsKeyPressed(CE_KEY_J)) m_ObjPos.x -= m_ObjMoveSpeed * dTime;
		else if (Celes::Input::IsKeyPressed(CE_KEY_L)) m_ObjPos.x += m_ObjMoveSpeed * dTime;
		if (Celes::Input::IsKeyPressed(CE_KEY_K)) m_ObjPos.y -= m_ObjMoveSpeed * dTime;
		else if (Celes::Input::IsKeyPressed(CE_KEY_I)) m_ObjPos.y += m_ObjMoveSpeed * dTime;

		Celes::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Celes::Renderer::Clear();

		m_Camera.SetPosition(m_CameraPos);
		m_Camera.SetViewDistance(m_CameraViewDist);

		auto transform = glm::translate(glm::mat4(1.0f), m_ObjPos) * glm::rotate(glm::mat4(1.0f), std::acosf(std::sqrtf(3.0f) / 3.0f), glm::vec3(1.0f, 0.0f, -1.0f));
		auto quadTransform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 2.0f));

		auto shader = m_ShaderLib.Get("Shadow");
		Celes::Renderer::GenShadowMap(m_Lights, { {m_VertexArray, transform}, {m_QuadVA, quadTransform} }, shader);

		Celes::Renderer::BeginScene(m_Camera);

		shader = m_ShaderLib.Get("Phong");
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->Bind();
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uKd", m_ObjColor);
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uLightPos", m_Lights[0]->GetLightPos());
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uCameraPos", m_CameraPos);
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformMat4("uLightVP", m_Lights[0]->GetLightVP());
		//m_Texture->Bind();
		m_Lights[0]->GetShadowMap()->Bind();
		std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformInt("uShadowMap", 0);

		Celes::Renderer::Submit(m_VertexArray, shader, transform);
		Celes::Renderer::Submit(m_QuadVA, shader, quadTransform);

		Celes::Renderer::EndScene();
	}

	void OnEvent(Celes::Event& e) override
	{
		//m_CameraController.OnEvent(e);
	}

	void OnGUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Object Color", &m_ObjColor.x);
		ImGui::End();
	}
private:
	Celes::ShaderLibrary m_ShaderLib;

	Celes::Ref<Celes::VertexArray> m_VertexArray, m_QuadVA;
	//Celes::Ref<Celes::Texture2D> m_Texture;

	Celes::PerspectiveCamera m_Camera;
	glm::vec3 m_CameraPos;
	float m_CameraViewDist = 2.0f;
	float m_CameraMoveSpeed = 2.0f;

	glm::vec3 m_ObjPos;
	float m_ObjMoveSpeed = 1.0f;
	glm::vec3 m_ObjColor = { 0.2f, 0.5f, 0.9f };

	std::vector<Celes::Ref<Celes::DirectionalLight>> m_Lights;
};

class Sandbox : public Celes::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		//PushLayer(new Sandbox2D());
		//PushOverlay(new Celes::GUILayer());
	}

	~Sandbox() {}
};

Celes::Application* Celes::CreateApp()
{
	return new Sandbox();
}
