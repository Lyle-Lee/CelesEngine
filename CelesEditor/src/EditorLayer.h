#pragma once

#include <Celes.h>

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
		//Celes::ShaderLibrary m_ShaderLib;

		//Celes::Ref<Celes::VertexArray> m_VertexArray;
		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_TextureStairs, m_TextureTree;

		OrthoCameraController m_CameraController;

		//glm::vec3 m_ObjPos;
		//float m_ObjMoveSpeed = 1.0f;
		glm::vec3 m_ObjColor = { 0.2f, 0.5f, 0.9f };

		struct ProfileResult
		{
			const char* Name;
			float Time;
		};
		std::vector<ProfileResult> m_ProfileResults;

		std::unordered_map<char, Ref<SubTexture2D>> m_TextureMap;

		Ref<FrameBuffer> m_FrameBuffer;
		Ref<Texture2D> m_FBColorAttachment;
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
	};

}