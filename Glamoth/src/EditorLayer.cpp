#include "EditorLayer.h"
#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>
//#include <Platform/OpenGL/OpenGLShader.h>
#include <gtc/matrix_transform.hpp>

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

namespace Celes {

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](float dTime) { m_ProfileResults.push_back({ name, dTime }); })

	extern const std::filesystem::path s_AssetsDirectory;

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(16.0f / 9.0f, true)//, m_ObjPos(0.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
#if 0
		m_Texture = Texture2D::Create("assets/textures/checkerbox.png");
		m_SpriteSheet = Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
		m_TextureStairs = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 7, 6 }, { 128, 128 });
		m_TextureTree = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1, 2 });

		m_TextureMap['d'] = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 6, 11 }, { 128, 128 });
		m_TextureMap['w'] = SubTexture2D::CreateFromCoord(m_SpriteSheet, { 11, 11 }, { 128, 128 });
#endif

		m_PlayIcon = Texture2D::Create("icons/ContentBrowser/icon-play.png");
		m_SimulateIcon = Texture2D::Create("icons/ContentBrowser/icon-simulate.png");
		m_StopIcon = Texture2D::Create("icons/ContentBrowser/icon-stop.png");

		m_CameraController.SetZoomLevel(5.0f);
		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		FrameBufferDescription fbDesc;
		fbDesc.Width = (uint32_t)m_ViewportSize.x;
		fbDesc.Height = (uint32_t)m_ViewportSize.y;
		fbDesc.AttachmentDesc = { TextureFormat::RGBA8, TextureFormat::R32INT, TextureFormat::DEPTH16 };
		m_FrameBuffer = FrameBuffer::Create(fbDesc);

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			OpenScene(sceneFilePath);
		}
		else
		{
			NewScene();
		}

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

		Renderer2D::SetLineWidth(4.0f);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(Celes::Timestep dTime)
	{
		//Timer timer("EditorLayer::OnUpdate", [&](auto profileRes) { m_ProfileResults.push_back(profileRes); });
		PROFILE_SCOPE("EditorLayer::OnUpdate");

		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(m_ViewportSize.x != m_FrameBuffer->GetWidth() || m_ViewportSize.y != m_FrameBuffer->GetHeight()))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.ResizeBounds(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		// Render
		Renderer2D::ResetStats();

		//Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		//Renderer::Clear();

		m_FrameBuffer->Bind();
		Celes::Renderer::ChangeViewport(m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight(), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

		// Clear our entity ID attachment to -1
		m_FrameBuffer->ClearAttachment(1, Entity::s_NullID);

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
		switch (m_SceneState)
		{
		case SceneState::Edit:
			//if (m_ViewportFocused)
			//	m_CameraController.OnUpdate(dTime);

			m_EditorCamera.OnUpdate(dTime);
			m_ActiveScene->OnUpdateEditor(dTime, m_EditorCamera);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdate(dTime);
			break;
		case SceneState::Simulate:
			//if (m_ViewportFocused)
			//	m_CameraController.OnUpdate(dTime);

			m_EditorCamera.OnUpdate(dTime);
			m_ActiveScene->OnUpdateSimulation(dTime, m_EditorCamera);
			break;
		default:
			CE_CORE_ERROR("Unknown scene state!");
			break;
		}

		//Celes::Renderer2D::EndScene();

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportMinBound.x;
		my = m_ViewportSize.y - (my - m_ViewportMinBound.y);
		if (mx >= 0.0f && mx < m_ViewportSize.x && my >= 0.0f && my < m_ViewportSize.y)
		{
			int pixelData = m_FrameBuffer->ReadPixel(1, (int)mx, (int)my);
			//CE_CORE_WARN("Pixel data = {0}", pixelData);
			m_HoveredEntity = pixelData == Entity::s_NullID ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::OnEvent(Celes::Event& e)
	{
		m_CameraController.OnEvent(e);
		if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
			m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressEvent>(std::bind(&EditorLayer::OnKeyPress, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonPressEvent>(std::bind(&EditorLayer::OnMouseButtonPress, this, std::placeholders::_1));
	}

	void EditorLayer::OnGUIRender() // TODO: split to different funcs.
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

				if (ImGui::MenuItem("Save", "Ctrl+S")) SaveScene();

				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) SaveSceneAs();

				if (ImGui::MenuItem("Open...", "Ctrl+O")) OpenScene();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsColliders);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SHPanel.OnGUIRender();
		m_CBPanel.OnGUIRender();

		ImGui::Begin("Stats");

		std::string name = "None";
		if ((bool)m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

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
		m_ViewportSize = { viewportSize.x, viewportSize.y };

		uint32_t texBufferID = m_FrameBuffer->GetAttachmentBufferID();
		ImGui::Image(reinterpret_cast<void*>(texBufferID), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

		auto viewportOffset = ImGui::GetWindowContentRegionMin(); // Tab bar size
		auto minBound = ImGui::GetWindowPos(); // Top left corner
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;
		m_ViewportMinBound = { minBound.x, minBound.y };

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(s_AssetsDirectory / path);
			}

			ImGui::EndDragDropTarget();
		}

		// Guizmos
		Entity selectedEntity = m_SHPanel.GetSelectedEntity();
		if (m_SceneState == SceneState::Edit && selectedEntity && m_GuizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportMinBound.x, m_ViewportMinBound.y, m_ViewportSize.x, m_ViewportSize.y);

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

		UIToolbar();

		ImGui::End();
	}

	void EditorLayer::UIToolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));
		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float iconSize = ImGui::GetWindowHeight() - 4.0f;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		{
			Ref<Texture2D> icon = m_SceneState == SceneState::Play ? m_StopIcon : m_PlayIcon;
			ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.5f - iconSize * 1.5f);
			if (ImGui::ImageButton((ImTextureID)icon->GetBufferID(), ImVec2(iconSize, iconSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), 0))
			{
				if (m_SceneState != SceneState::Play)
					OnScenePlay();
				else
					OnSceneStop();
			}
		}
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = m_SceneState == SceneState::Simulate ? m_StopIcon : m_SimulateIcon;
			ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.5f + iconSize * 0.5f);
			if (ImGui::ImageButton((ImTextureID)icon->GetBufferID(), ImVec2(iconSize, iconSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), 0))
			{
				if (m_SceneState != SceneState::Simulate)
					OnSceneSimulate();
				else
					OnSceneStop();
			}
		}

		ImGui::PopStyleColor();
		ImGui::End();

		ImGui::PopStyleVar(2);
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		// Copy the current editor scene
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
		m_SHPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		// Copy the current editor scene
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();
		m_SHPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		switch (m_SceneState)
		{
		case SceneState::Play:
			m_ActiveScene->OnRuntimeStop();
			break;
		case SceneState::Simulate:
			m_ActiveScene->OnSimulationStop();
			break;
		default:
			CE_CORE_ASSERT(false, "Invalid scene state.")
			break;
		}

		m_SceneState = SceneState::Edit;

		// Release memory and reset to editor scene
		m_ActiveScene = m_EditorScene;
		m_SHPanel.SetContext(m_ActiveScene);
	}

	bool EditorLayer::OnKeyPress(KeyPressEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat()) return false;

		bool ctrl = Input::IsKeyPressed(CE_KEY_LEFT_CONTROL) || Input::IsKeyPressed(CE_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(CE_KEY_LEFT_SHIFT) || Input::IsKeyPressed(CE_KEY_RIGHT_SHIFT);

		switch (e.GetKeyCode())
		{
			case CE_KEY_N:
				if (ctrl) NewScene();
				break;
			case CE_KEY_S:
				if (ctrl)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}
				break;
			case CE_KEY_O:
				if (ctrl) OpenScene();
				break;
			// Scene commands
			case CE_KEY_D:
				if (ctrl) DuplicateEntity();
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

	bool EditorLayer::OnMouseButtonPress(MouseButtonPressEvent& e)
	{
		if (m_ViewportHovered && e.GetMouseButton() == CE_MOUSE_BUTTON_LEFT && !ImGuizmo::IsOver() && !Input::IsKeyPressed(CE_KEY_LEFT_ALT))
			m_SHPanel.SetSelectedEntity(m_HoveredEntity);

		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		glm::vec3 viewDir = glm::vec3(0.0f, 0.0f, 1.0f);
		if (m_SceneState == SceneState::Play)
		{
			Entity cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			if (!cameraEntity)
				return;
			
			glm::mat4 viewMat = cameraEntity.GetComponent<TransformComponent>().GetTransform();
			viewDir = viewMat * glm::vec4(viewDir, 0.0f);
			Renderer2D::BeginScene(cameraEntity.GetComponent<CameraComponent>().Camera, viewMat);
		}
		else
		{
			viewDir = m_EditorCamera.GetViewMat() * glm::vec4(viewDir, 0.0f);
			Renderer2D::BeginScene(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			// Box colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [transformCompo, bc2dCompo] = view.get<TransformComponent, BoxCollider2DComponent>(entity);
					glm::vec3 translation = transformCompo.Translation + glm::vec3(bc2dCompo.Offset, 0.0f) + viewDir * 0.001f;
					glm::vec3 scale = transformCompo.Scale * glm::vec3(bc2dCompo.Size * 2.0f, 1.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), transformCompo.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}

			// Circle colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [transformCompo, cc2dCompo] = view.get<TransformComponent, CircleCollider2DComponent>(entity);
					glm::vec3 translation = transformCompo.Translation + glm::vec3(cc2dCompo.Offset, 0.0f) + viewDir * 0.005f;
					glm::vec3 scale = transformCompo.Scale * cc2dCompo.Radius * 2.0f;
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.01f);
				}
			}
		}

		// Draw outline of the selected entity
		if (auto selectedEntity = m_SHPanel.GetSelectedEntity())
		{
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::NewScene()
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		m_SHPanel.SetContext(m_ActiveScene);
		m_CurrentScenePath = std::filesystem::path();
	}

	void EditorLayer::SaveScene()
	{
		if (!m_CurrentScenePath.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(m_CurrentScenePath.string());
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Celes Scene (*.celes)\0*.celes\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(filepath);
			m_CurrentScenePath = filepath;
		}
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Celes Scene (*.celes)\0*.celes\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension() != ".celes")
		{
			CE_WARN("Could not load '{0}' - not a scene file.", path.string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_ActiveScene = m_EditorScene;

			m_SHPanel.SetContext(m_ActiveScene);
			m_CurrentScenePath = path;
		}
	}

	void EditorLayer::DuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		if (Entity selectedEntity = m_SHPanel.GetSelectedEntity())
			m_ActiveScene->DuplicateEntity(selectedEntity);
	}

}
