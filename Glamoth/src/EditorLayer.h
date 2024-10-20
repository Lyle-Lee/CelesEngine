#pragma once

#include <Celes.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace Celes {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach()	override;
		void OnUpdate(Timestep dTime) override;
		void OnEvent(Event& e) override;
		void OnGUIRender() override;
	private:
		// UI panels
		void UIToolbar();

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		bool OnKeyPress(KeyPressEvent& e);
		bool OnMouseButtonPress(MouseButtonPressEvent& e);
		void OnOverlayRender();

		void NewScene();
		void SaveScene();
		void SaveSceneAs();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);

		void DuplicateEntity();

		//Celes::ShaderLibrary m_ShaderLib;

		//Celes::Ref<Celes::VertexArray> m_VertexArray;
#if 0
		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_TextureStairs, m_TextureTree;
		std::unordered_map<char, Ref<SubTexture2D>> m_TextureMap;
#endif

		OrthoCameraController m_CameraController;
		EditorCamera m_EditorCamera;

		//glm::vec3 m_ObjPos;
		//float m_ObjMoveSpeed = 1.0f;
		//glm::vec3 m_ObjColor = { 0.2f, 0.5f, 0.9f };

		int m_GuizmoType = -1;

		bool m_ShowPhysicsColliders = false;

		struct ProfileResult
		{
			const char* Name;
			float Time;
		};
		std::vector<ProfileResult> m_ProfileResults;

		Ref<FrameBuffer> m_FrameBuffer;
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
		glm::vec2 m_ViewportMinBound = { 0.0f, 0.0f };
		bool m_ViewportFocused = false, m_ViewportHovered = false;

		Entity m_SquareEntity;
		Entity m_CameraEntity1, m_CameraEntity2;
		Entity m_HoveredEntity;
		//bool m_MainCamera = true;

		Ref<Scene> m_ActiveScene, m_EditorScene;
		std::filesystem::path m_CurrentScenePath;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SHPanel;
		ContentBrowserPanel m_CBPanel;

		// Editor resources
		Ref<Texture2D> m_PlayIcon;
		Ref<Texture2D> m_SimulateIcon;
		Ref<Texture2D> m_StopIcon;
	};

}
