#include "EditorLayer.h"
#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>
//#include <Platform/OpenGL/OpenGLShader.h>
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

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
#if 0
		m_SquareEntity = m_ActiveScene->CreateEntity("Square1");
		m_SquareEntity.AddComponent<SpriteRenderComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		auto redSquare = m_ActiveScene->CreateEntity("Square2");
		redSquare.AddComponent<SpriteRenderComponent>(glm::vec4(1.0, 0.0f, 0.0f, 1.0f));

		m_CameraEntity1 = m_ActiveScene->CreateEntity("Camera A");
		m_CameraEntity1.AddComponent<CameraComponent>();

		m_CameraEntity2 = m_ActiveScene->CreateEntity("Camera B");
		auto& cameraCompo = m_CameraEntity2.AddComponent<CameraComponent>();
		cameraCompo.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate() override {}
			void OnDestroy() override {}

			void OnUpdate(Timestep dTime) override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;

				if (Input::IsKeyPressed(CE_KEY_A)) translation.x -= speed * dTime;
				if (Input::IsKeyPressed(CE_KEY_D)) translation.x += speed * dTime;
				if (Input::IsKeyPressed(CE_KEY_S)) translation.y -= speed * dTime;
				if (Input::IsKeyPressed(CE_KEY_W)) translation.y += speed * dTime;
			}
		};

		m_CameraEntity1.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif
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
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		if (m_ViewportFocused)
		{
			m_CameraController.OnUpdate(dTime);
			m_EditorCamera.OnUpdate(dTime);
		}

		// Render
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
		// Update scene
		m_ActiveScene->OnUpdateEditor(dTime, m_EditorCamera);

		//Celes::Renderer2D::EndScene();

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::OnEvent(Celes::Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressEvent>(std::bind(&EditorLayer::OnKeyPress, this, std::placeholders::_1));
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

		// Dock space
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWindowSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWindowSize;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N")) NewScene();

				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) SaveSceneAs();

				if (ImGui::MenuItem("Open...", "Ctrl+O")) OpenScene();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SHPanel.OnGUIRender();

		ImGui::Begin("Stats");

		auto stats = Celes::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCallsCnt);
		ImGui::Text("Quads: %d", stats.QuadCnt);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCnt());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCnt());

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
		Application::Get().GetGUILayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportSize))
			m_ViewportSize = { viewportSize.x, viewportSize.y };

		uint32_t texBufferID = m_FBColorAttachment->GetBufferID();
		ImGui::Image(reinterpret_cast<void*>(texBufferID), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

		// Guizmos
		Entity selectedEntity = m_SHPanel.GetSelectedEntity();
		if (selectedEntity && m_GuizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			// Runtime camera from entity
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			// const glm::mat4& cameraProj = camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			// Editor camera
			const glm::mat4& cameraProj = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMat();

			// Entity transform
			auto& transformCompo = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = transformCompo.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(CE_KEY_LEFT_CONTROL);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(&cameraView[0].x, &cameraProj[0].x, (ImGuizmo::OPERATION)m_GuizmoType, ImGuizmo::LOCAL, &transform[0].x, nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - transformCompo.Rotation;
				transformCompo.Translation = translation;
				transformCompo.Rotation += deltaRotation;
				transformCompo.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	bool EditorLayer::OnKeyPress(KeyPressEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0) return false;

		bool ctrl = Input::IsKeyPressed(CE_KEY_LEFT_CONTROL) || Input::IsKeyPressed(CE_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(CE_KEY_LEFT_SHIFT) || Input::IsKeyPressed(CE_KEY_RIGHT_SHIFT);

		switch (e.GetKeyCode())
		{
			case CE_KEY_N:
				if (ctrl) NewScene();
				break;
			case CE_KEY_S:
				if (ctrl && shift) SaveSceneAs();
				break;
			case CE_KEY_O:
				if (ctrl) OpenScene();
				break;
			// Guizmos
			case CE_KEY_Q:
				m_GuizmoType = -1;
				break;
			case CE_KEY_W:
				m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case CE_KEY_E:
				m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case CE_KEY_R:
				m_GuizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			default:
				break;
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SHPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Celes Scene (*.celes)\0*.celes\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Celes Scene (*.celes)\0*.celes\0");
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SHPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

}
