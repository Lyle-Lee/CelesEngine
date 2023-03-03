#include "Sandbox2D.h"
#include <ImGui/imgui.h>
#include <Platform/OpenGL/OpenGLShader.h>
#include <gtc/matrix_transform.hpp>

Sandbox2D::Sandbox2D(): Layer("Sandbox2D"), m_CameraController(16.0f / 9.0f, true), m_ObjPos(0.0f)
{
}

void Sandbox2D::OnAttach()
{
	m_VertexArray = Celes::VertexArray::Create();

	// Square
	float vertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	Celes::Ref<Celes::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(Celes::VertexBuffer::Create(vertices, sizeof(vertices)));

	vertexBuffer->SetLayout({
		{"aPosition", Celes::ShaderDataType::Float3},
		{"aTexCoord", Celes::ShaderDataType::Float2}
		});
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	// Square
	uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };

	Celes::Ref<Celes::IndexBuffer> indexBuffer;
	indexBuffer.reset(Celes::IndexBuffer::Create(indices, sizeof(indices)));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	auto shader = m_ShaderLib.Load("assets/shaders/TestVertexShader.glsl", "assets/shaders/TestFragmentShader.glsl");

	m_Texture = Celes::Texture2D::Create("assets/textures/checkerbox.png");
	std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->Bind();
	std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformInt("uTexture", 0);
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Celes::Timestep dTime)
{
	m_CameraController.OnUpdate(dTime);

	if (Celes::Input::IsKeyPressed(CE_KEY_J)) m_ObjPos.x -= m_ObjMoveSpeed * dTime;
	else if (Celes::Input::IsKeyPressed(CE_KEY_L)) m_ObjPos.x += m_ObjMoveSpeed * dTime;
	if (Celes::Input::IsKeyPressed(CE_KEY_K)) m_ObjPos.y -= m_ObjMoveSpeed * dTime;
	else if (Celes::Input::IsKeyPressed(CE_KEY_I)) m_ObjPos.y += m_ObjMoveSpeed * dTime;

	Celes::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Celes::Renderer::Clear();

	Celes::Renderer::BeginScene(m_CameraController.GetCamera());

	auto shader = m_ShaderLib.Get("Test");
	std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->Bind();
	//std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->SetUniformFloat3("uColor", m_ObjColor);
	m_Texture->Bind();

	Celes::Renderer::Submit(m_VertexArray, shader, glm::translate(glm::mat4(1.0f), m_ObjPos));

	Celes::Renderer::EndScene();
}

void Sandbox2D::OnEvent(Celes::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnGUIRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Object Color", &m_ObjColor.x);
	ImGui::End();
}
