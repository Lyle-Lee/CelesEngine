#include "Sandbox2D.h"
#include <ImGui/imgui.h>
#include <Platform/OpenGL/OpenGLShader.h>
#include <gtc/matrix_transform.hpp>

Sandbox2D::Sandbox2D(): Layer("Sandbox2D"), m_CameraController(16.0f / 9.0f, true), m_ObjPos(0.0f)
{
}

void Sandbox2D::OnAttach()
{
	m_Texture = Celes::Texture2D::Create("assets/textures/checkerbox.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Celes::Timestep dTime)
{
	m_CameraController.OnUpdate(dTime);

	/*if (Celes::Input::IsKeyPressed(CE_KEY_J)) m_ObjPos.x -= m_ObjMoveSpeed * dTime;
	else if (Celes::Input::IsKeyPressed(CE_KEY_L)) m_ObjPos.x += m_ObjMoveSpeed * dTime;
	if (Celes::Input::IsKeyPressed(CE_KEY_K)) m_ObjPos.y -= m_ObjMoveSpeed * dTime;
	else if (Celes::Input::IsKeyPressed(CE_KEY_I)) m_ObjPos.y += m_ObjMoveSpeed * dTime;*/

	Celes::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Celes::Renderer::Clear();

	//Celes::Renderer::BeginScene(m_CameraController.GetCamera());
	Celes::Renderer2D::BeginScene(m_CameraController.GetCamera());

	//auto shader = m_ShaderLib.Get("Test");
	//std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->Bind();
	//m_Texture->Bind();

	//Celes::Renderer::Submit(m_VertexArray, shader, glm::translate(glm::mat4(1.0f), m_ObjPos));

	Celes::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.5f, 0.9f, 1.0f });
	Celes::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Celes::Renderer2D::DrawQuad({ 0.2f, 0.5f, -0.1f }, { 0.5f, 0.5f }, m_Texture);

	//Celes::Renderer::EndScene();
	Celes::Renderer2D::EndScene();
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
