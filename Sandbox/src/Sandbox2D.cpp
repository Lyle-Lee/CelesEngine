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
	m_SpriteSheet = Celes::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
	m_TextureStairs = Celes::SubTexture2D::CreateFromCoord(m_SpriteSheet, { 7, 6 }, { 128, 128 });
	m_TextureTree = Celes::SubTexture2D::CreateFromCoord(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1, 2 });

	m_ParticleInfo.ColorBegin = { 254.0f / 255.0f, 212.0f / 255.0f, 123.0f / 255.0f, 1.0f };
	m_ParticleInfo.ColorEnd = { 254.0f / 255.0f, 109.0f / 255.0f, 41.0f / 255.0f, 1.0f };
	m_ParticleInfo.SizeBegin = 0.5f, m_ParticleInfo.SizeVariation = 0.3f, m_ParticleInfo.SizeEnd = 0.0f;
	m_ParticleInfo.LifeTime = 1.0f;
	m_ParticleInfo.Position = { 0.0f, 0.0f };
	m_ParticleInfo.Velocity = { 0.0f, 0.0f };
	m_ParticleInfo.VelocityVariation = { 3.0f, 1.0f };
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

	Celes::Renderer2D::ResetStats();

	Celes::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Celes::Renderer::Clear();
#if 0
	static float rotation = 0.0f;
	rotation += dTime * 20.0f;

	//Celes::Renderer::BeginScene(m_CameraController.GetCamera());
	Celes::Renderer2D::BeginScene(m_CameraController.GetCamera());

	//auto shader = m_ShaderLib.Get("Test");
	//std::dynamic_pointer_cast<Celes::OpenGLShader>(shader)->Bind();
	//m_Texture->Bind();

	//Celes::Renderer::Submit(m_VertexArray, shader, glm::translate(glm::mat4(1.0f), m_ObjPos));

	Celes::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.5f, 0.9f, 1.0f });
	Celes::Renderer2D::DrawRotatedQuad({ -1.0f, 1.0f }, { 0.8f, 0.8f }, glm::radians(45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
	Celes::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Celes::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f);
	Celes::Renderer2D::DrawRotatedQuad({ -0.5f, -0.5f, -0.1}, { 1.0f, 1.0f }, glm::radians(rotation), m_Texture, 20.0f);

	//Celes::Renderer::EndScene();
	Celes::Renderer2D::EndScene();
#endif
	if (Celes::Input::IsMouseBottonPressed(CE_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Celes::Input::GetMousePos();
		auto width = Celes::Application::Get().GetWindow().GetWidth();
		auto height = Celes::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_ParticleInfo.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; ++i) m_ParticleSystem.Emit(m_ParticleInfo);
	}

	Celes::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Celes::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStairs);
	Celes::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);
	Celes::Renderer2D::EndScene();

	m_ParticleSystem.OnUpdate(dTime);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnEvent(Celes::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnGUIRender()
{
	ImGui::Begin("Settings");

	auto stats = Celes::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCallsCnt);
	ImGui::Text("Quads: %d", stats.QuadCnt);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCnt());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCnt());

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
