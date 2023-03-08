#include "PCH.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Platform/OpenGL/OpenGLCommand.h"
#include <gtc/matrix_transform.hpp>

namespace Celes {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		//Ref<Shader> FlatColorShader;
		Ref<Shader> TestShader;
		Ref<Texture> BlankTexture;
	};

	static Renderer2DStorage* s_Data;
	Scope<RenderCommand> Renderer2D::s_Cmd = CreateScope<OpenGLCommand>();

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();

		s_Data->QuadVertexArray = VertexArray::Create();

		// Square
		float vertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		vertexBuffer->SetLayout({
			{"aPosition", ShaderDataType::Float3},
			{"aTexCoord", ShaderDataType::Float2}
			});
		s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

		// Square
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));
		s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

		s_Data->BlankTexture = Texture2D::Create(1, 1);
		uint32_t blankData = 0xffffffff;
		s_Data->BlankTexture->SetData(&blankData, sizeof(uint32_t));

		//s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColorVertexShader.glsl", "assets/shaders/FlatColorFragmentShader.glsl", true);
		s_Data->TestShader = Shader::Create("assets/shaders/TestVertexShader.glsl", "assets/shaders/TestFragmentShader.glsl", true);
		s_Data->TestShader->Bind();
		s_Data->TestShader->SetInt("uTexture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthoCamera& camera)
	{
		//s_Data->FlatColorShader->Bind();
		//s_Data->FlatColorShader->SetMat4("uViewProj", camera.GetVP());

		s_Data->TestShader->Bind();
		s_Data->TestShader->SetMat4("uViewProj", camera.GetVP());
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->TestShader->SetFloat4("uColor", color);
		s_Data->TestShader->SetFloat("uTilingFactor", 1.0f);
		s_Data->BlankTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TestShader->SetMat4("uModel", transform);

		s_Data->QuadVertexArray->Bind();
		s_Cmd->DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		s_Data->TestShader->SetFloat4("uColor", glm::vec4(1.0f));
		s_Data->TestShader->SetFloat("uTilingFactor", tilingFactor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TestShader->SetMat4("uModel", transform);

		s_Data->QuadVertexArray->Bind();
		s_Cmd->DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		s_Data->TestShader->SetFloat4("uColor", color);
		s_Data->TestShader->SetFloat("uTilingFactor", 1.0f);
		s_Data->BlankTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TestShader->SetMat4("uModel", transform);

		s_Data->QuadVertexArray->Bind();
		s_Cmd->DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
	{
		DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, rotation, texture, tilingFactor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
	{
		s_Data->TestShader->SetFloat4("uColor", glm::vec4(1.0f));
		s_Data->TestShader->SetFloat("uTilingFactor", tilingFactor);
		texture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TestShader->SetMat4("uModel", transform);

		s_Data->QuadVertexArray->Bind();
		s_Cmd->DrawIndexed(s_Data->QuadVertexArray);
	}

}
