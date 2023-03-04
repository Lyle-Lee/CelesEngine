#pragma once

#include <glm.hpp>
#include "VertexArray.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"

namespace Celes {

	enum class GraphicsAPI
	{
		None = 0, OpenGL = 1, Direct3D = 2
	};

	class CE_API RenderCommand
	{
	public:
		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& va) = 0;
	};

	class CE_API Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		inline static void SetClearColor(const glm::vec4& color) { s_Cmd->SetClearColor(color); }
		inline static void Clear() { s_Cmd->Clear(); }

		static void BeginScene(OrthoCamera& camera);
		static void BeginScene(PerspectiveCamera& camera);
		static void EndScene();
		static void Submit(const Ref<VertexArray>& va, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));
		static void GenShadowMap(const std::vector<Ref<DirectionalLight>>& lights, const std::vector<std::pair<Ref<VertexArray>, glm::mat4>>& objects, const Ref<Shader>& shader);

		inline static GraphicsAPI GetAPI() { return s_GAPI; }
	private:
		static GraphicsAPI s_GAPI;
		static Scope<RenderCommand> s_Cmd;

		struct SceneData
		{
			glm::mat4 VP;
			int windowWidth = 1280, windowHeight = 720;
		};

		static Scope<SceneData> s_SceneData;
	};

}
