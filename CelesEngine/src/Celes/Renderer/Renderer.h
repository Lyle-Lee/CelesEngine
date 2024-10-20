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
		virtual ~RenderCommand() = default;

		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCnt = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& va, uint32_t vertexCnt) = 0;
	};

	class Renderer
	{
	public:
		CE_API static void Init();
		CE_API static void OnWindowResize(uint32_t width, uint32_t height);

		CE_API inline static void SetClearColor(const glm::vec4& color) { s_Cmd->SetClearColor(color); }
		CE_API inline static void Clear() { s_Cmd->Clear(); }
		CE_API static void ChangeViewport(uint32_t width, uint32_t height, const glm::vec4& clearColor);

		CE_API static void BeginScene(OrthoCamera& camera);
		CE_API static void BeginScene(PerspectiveCamera& camera);
		CE_API static void EndScene();

		CE_API static void Submit(const Ref<VertexArray>& va, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));
		CE_API static void GenShadowMap(const std::vector<Ref<DirectionalLight>>& lights, const std::vector<std::pair<Ref<VertexArray>, glm::mat4>>& objects, const Ref<Shader>& shader);

		CE_API inline static GraphicsAPI GetAPI() { return s_GAPI; }
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
