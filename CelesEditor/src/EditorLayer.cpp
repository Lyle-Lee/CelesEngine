#include "EditorLayer.h"
#include <ImGui/imgui.h>
#include <Platform/OpenGL/OpenGLShader.h>
#include <gtc/matrix_transform.hpp>
#include <chrono>

static const uint32_t s_MapWidth = 24;
static const uint32_t s_MapHeight = 12;
static const char* s_MapTiles = "wwwwwwwwwwwwwwwwwwwwwwww"
								"wwwwwwwddddddwwwwwwwwwww"
								"wwwwwdddddddddddwwwwwwww"
								"wwwwdddddddddddddddwwwww"
								"wwwdddddddddddddddddwwww"
								"wwddddwwwdddddddddddwwww"
								"wdddddwwwddddddddddwwwww"
								"wwddddddddddddddddwwwwww"
								"wwwddddddddddddddwwwwwww"
								"wwwwwwdddddddddwwwwwwdww"
								"wwwwwwwwddddwwwwwwwwwwww"
								"wwwwwwwwwwwwwwwwwwwwwwww";

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

namespace Celes {

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(16.0f / 9.0f, true)//, m_ObjPos(0.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
		m_Texture = Texture2D::Create("assets/textures/checkerbox.png");
		m_SpriteSheet = Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
		m_TextureStairs = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 7, 6 }, { 128, 128 });
		m_TextureTree = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1, 2 });

		m_TextureMap['d'] = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 6, 11 }, { 128, 128 });
		m_TextureMap['w'] = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 11, 11 }, { 128, 128 });

		m_CameraController.SetZoomLevel(5.0f);

		m_FrameBuffer = FrameBuffer::Create((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_FBColorAttachment = Texture2D::Create(m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight());
		m_FBColorAttachment->Bind();
		m_FrameBuffer->AddAttachment(m_FBColorAttachment);
		m_FrameBuffer->SetRenderBuffer();
		m_FrameBuffer->Unbind();

		m_ActiveScene = CreateRef<Scene>();
		m_SquareEntity = m_ActiveScene->CreateEntity("Square");
		m_SquareEntity.AddComponent<SpriteRenderComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		m_CameraEntity1 = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity1.AddComponent<CameraComponent>();

		m_CameraEntity2 = m_ActiveScene->CreateEntity("Clip-Space Entity");
		auto& cameraCompo = m_CameraEntity2.AddComponent<CameraComponent>();
		cameraCompo.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate() override {}
			void OnDestroy() override {}

			void OnUpdate(Timestep dTime) override
			{
				auto& transform = GetComponent<TransformComponent>().Transform;
				float speed = 5.0f;

				if (Input::IsKeyPressed(CE_KEY_A)) transform[3][0] -= speed * dTime;
				if (Input::IsKeyPressed(CE_KEY_D)) transform[3][0] += speed * dTime;
				if (Input::IsKeyPressed(CE_KEY_S)) transform[3][1] -= speed * dTime;
				if (Input::IsKeyPressed(CE_KEY_W)) transform[3][1] += speed * dTime;
			}
		};

		m_CameraEntity1.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SHPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Celes::Timestep dTime)
	{
		//Timer timer("EditorLayer::OnUpdate", [&](auto profileRes) { m_ProfileResults.push_back(profileRes); });
		PROFILE_SCOPE("EditorLayer::OnUpdate");

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(m_ViewportSize.x != m_FrameBuffer->GetWidth() || m_ViewportSize.y != m_FrameBuffer->GetHeight()))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.ResizeBounds(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		if (m_ViewportFocused) m_CameraController.OnUpdate(dTime);

		Renderer2D::ResetStats();

		//Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		//Renderer::Clear();

		m_FrameBuffer->Bind();
		Celes::Renderer::ChangeViewport(m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight(), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

#if 0
		Celes::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Celes::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStairs);
		//Celes::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);

		for (uint32_t y = 0; y < s_MapHeight; ++y)
		{
			for (uint32_t x = 0; x < s_MapWidth; ++x)
			{
				char tileType = s_MapTiles[y * s_MapWidth + x];
				Celes::Ref<Celes::SubTexture2D> targetTexture;
				auto it = m_TextureMap.find(tileType);
				if (it != m_TextureMap.end())
					targetTexture = it->second;
				else
					targetTexture = m_TextureTree;

				Celes::Renderer2D::DrawQuad({ (float)x - (float)s_MapWidth / 2.0f, (float)s_MapHeight / 2.0f - (float)y }, { 1.0f, 1.0f }, targetTexture);
			}
		}
#endif
		m_ActiveScene->OnUpdate(dTime);

		//Celes::Renderer2D::EndScene();

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::OnEvent(Celes::Event& e)
	{
		m_CameraController.OnEvent(e);
	}

	void EditorLayer::OnGUIRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen) ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SHPanel.OnGUIRender();

		ImGui::Begin("Settings");

		auto stats = Celes::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCallsCnt);
		ImGui::Text("Quads: %d", stats.QuadCnt);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCnt());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCnt());

		if (m_SquareEntity)
		{
			ImGui::Separator();
			ImGui::Text("%s", m_SquareEntity.GetComponent<TagComponent>().Tag.c_str());

			auto& squareColor = m_SquareEntity.GetComponent<SpriteRenderComponent>().Color;
			ImGui::ColorEdit3("Object Color", &squareColor.x);
			ImGui::Separator();
		}

		if (ImGui::Checkbox("Main Camera", &m_MainCamera))
		{
			m_CameraEntity1.GetComponent<CameraComponent>().Primary = m_MainCamera;
			m_CameraEntity2.GetComponent<CameraComponent>().Primary = !m_MainCamera;
		}

		{
			auto& camera = m_CameraEntity2.GetComponent<CameraComponent>().Camera;
			float orthoSize = camera.GetOrthoSize();
			if (ImGui::DragFloat("Clip Camera Ortho Size", &orthoSize))
				camera.SetOrthoSize(orthoSize);
		}

		for (auto& result : m_ProfileResults)
		{
			char label[50];
			strcpy_s(label, result.Name);
			strcat_s(label, "  %.3fms");
			ImGui::Text(label, result.Time);
		}
		m_ProfileResults.clear();
		
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetGUILayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportSize))
			m_ViewportSize = { viewportSize.x, viewportSize.y };

		uint32_t texBufferID = m_FBColorAttachment->GetBufferID();
		ImGui::Image((void*)texBufferID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

}
