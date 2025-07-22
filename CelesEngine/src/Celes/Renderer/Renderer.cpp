#include "PCH.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"

namespace Celes {

	GraphicsAPI Renderer::s_GAPI = GraphicsAPI::OpenGL;
	Scope<RenderCommand> Renderer::s_Cmd = CreateScope<OpenGLCommand>();
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		s_Cmd->Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_Cmd->SetViewport(0, 0, width, height);
		s_SceneData->windowWidth = width;
		s_SceneData->windowHeight = height;
	}

	void Renderer::ChangeViewport(uint32_t width, uint32_t height, const glm::vec4& clearColor)
	{
		s_Cmd->SetViewport(0, 0, width, height);
		SetClearColor(clearColor);
		Clear();
	}

	void Renderer::BeginScene(OrthoCamera& camera)
	{
		s_SceneData->VP = camera.GetVP();
	}

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		s_SceneData->VP = camera.GetVP();

		ChangeViewport(s_SceneData->windowWidth, s_SceneData->windowHeight, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<VertexArray>& va, const Ref<Shader>& shader, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("uViewProj", s_SceneData->VP);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("uModel", transform);

		va->Bind();
		s_Cmd->DrawIndexed(va);
	}

	void Renderer::GenShadowMap(const std::vector<Ref<DirectionalLight>>& lights, const std::vector<std::pair<Ref<VertexArray>, glm::mat4>>& objects, const Ref<Shader>& shader)
	{
		for (auto& light : lights)
		{
			CE_CORE_ASSERT(light->GetFrameBuffer(), "Could not find shadow map!")

			light->GetFrameBuffer()->Bind();
			ChangeViewport(light->GetFrameBuffer()->GetWidth(), light->GetFrameBuffer()->GetHeight(), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

			shader->Bind();
			std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("uLightVP", light->GetLightVP());

			for (auto& [va, transform] : objects)
			{
				std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("uModel", transform);

				va->Bind();
				s_Cmd->DrawIndexed(va);
			}

			light->GetFrameBuffer()->Unbind();
		}
	}

}
