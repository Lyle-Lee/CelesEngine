#include "Sandbox2D.h"
#include <ImGui/imgui.h>
#include <Platform/OpenGL/OpenGLShader.h>
#include <gtc/matrix_transform.hpp>
#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func): m_Name(name), m_Stopped(false), m_Func(func)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped) Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		//std::cout << m_Name << ": " << duration << "ms" << std::endl;
		m_Func({ m_Name, duration });
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
	Fn m_Func;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileRes) { m_ProfileResults.push_back(profileRes); })

Sandbox2D::Sandbox2D(): Layer("Sandbox2D"), m_CameraController(16.0f / 9.0f, true)//, m_ObjPos(0.0f)
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
	//Timer timer("Sandbox2D::OnUpdate", [&](auto profileRes) { m_ProfileResults.push_back(profileRes); });
	PROFILE_SCOPE("Sandbox2D::OnUpdate");

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
	Celes::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
	Celes::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f);

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

	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy_s(label, result.Name);
		strcat_s(label, "  %.3fms");
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();

	ImGui::End();
}
