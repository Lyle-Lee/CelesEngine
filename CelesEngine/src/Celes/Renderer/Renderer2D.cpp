#include "PCH.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Platform/OpenGL/OpenGLCommand.h"
#include <gtc/matrix_transform.hpp>

namespace Celes {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexID;
		float TilingFactor;
	};

	struct Renderer2DStorage
	{
		static const uint32_t MaxQuadsCnt = 10000;
		static const uint32_t MaxVerticesCnt = MaxQuadsCnt * 4;
		static const uint32_t MaxIndicesCnt = MaxQuadsCnt * 6;
		static const uint32_t MaxTextureSlots = 32;

		uint32_t QuadIndexCnt = 0;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		//Ref<Shader> FlatColorShader;
		Ref<Shader> TestShader;
		Ref<Texture> BlankTexture;

		QuadVertex* QuadVBBase = nullptr;
		QuadVertex* QuadVBPtr = nullptr;

		std::array<Ref<Texture>, MaxTextureSlots> TexSlots;
		uint32_t TexIndex = 1; // 0 = blank texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};

	static Renderer2DStorage s_Data;
	Scope<RenderCommand> Renderer2D::s_Cmd = CreateScope<OpenGLCommand>();

	void Renderer2D::Init()
	{
		//s_Data = new Renderer2DStorage();

		s_Data.QuadVertexArray = VertexArray::Create();

		// Square
		/*float vertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};*/
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; // x, y, z, w
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		//Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticesCnt * sizeof(QuadVertex));

		s_Data.QuadVertexBuffer->SetLayout({
			{"aPosition", ShaderDataType::Float3},
			{"aColor", ShaderDataType::Float4},
			{"aTexCoord", ShaderDataType::Float2},
			{"aTexID", ShaderDataType::Float},
			{"aTilingFactor", ShaderDataType::Float}
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVBBase = new QuadVertex[s_Data.MaxVerticesCnt];

		// Square
		//uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndicesCnt];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndicesCnt; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndicesCnt * sizeof(uint32_t));
		s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);

		delete[] quadIndices;

		s_Data.BlankTexture = Texture2D::Create(1, 1);
		uint32_t blankData = 0xffffffff;
		s_Data.BlankTexture->SetData(&blankData, sizeof(uint32_t));

		int samplers[s_Data.MaxTextureSlots];
		for (int i = 0; i < s_Data.MaxTextureSlots; ++i) samplers[i] = i;

		//s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColorVertexShader.glsl", "assets/shaders/FlatColorFragmentShader.glsl", true);
		s_Data.TestShader = Shader::Create("assets/shaders/TestVertexShader.glsl", "assets/shaders/TestFragmentShader.glsl", true);
		s_Data.TestShader->Bind();
		s_Data.TestShader->SetIntArray("uTexture", samplers, s_Data.MaxTextureSlots);

		s_Data.TexSlots[0] = s_Data.BlankTexture;
	}

	void Renderer2D::Shutdown()
	{
		//delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthoCamera& camera)
	{
		//s_Data.FlatColorShader->Bind();
		//s_Data.FlatColorShader->SetMat4("uViewProj", camera.GetVP());

		s_Data.TestShader->Bind();
		s_Data.TestShader->SetMat4("uViewProj", camera.GetVP());

		s_Data.QuadIndexCnt = 0;
		s_Data.QuadVBPtr = s_Data.QuadVBBase;

		s_Data.TexIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint8_t*)s_Data.QuadVBPtr - (uint8_t*)s_Data.QuadVBBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVBBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		for (uint32_t i = 0; i < s_Data.TexIndex; ++i)
			s_Data.TexSlots[i]->Bind(i);

		s_Cmd->DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCnt);

		s_Data.Stats.DrawCallsCnt++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ pos.x, pos.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor)
	{
		DrawQuad({ pos.x, pos.y, 0.0f }, size, subTexture, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		float texID = 0.0f;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == subTexture->GetTexture()->GetBufferID())
			{
				texID = (float)i;
				break;
			}
		}

		if (texID == 0.0f)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = (float)s_Data.TexIndex;
			s_Data.TexSlots[s_Data.TexIndex++] = subTexture->GetTexture();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		const glm::vec2* texCoords = subTexture->GetTexCoords();

		for (int i = 0; i < 4; ++i)
		{
			s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVBPtr->Color = glm::vec4(1.0f);
			s_Data.QuadVBPtr->TexCoord = texCoords[i];
			s_Data.QuadVBPtr->TexID = texID;
			s_Data.QuadVBPtr->TilingFactor = tilingFactor;
			s_Data.QuadVBPtr++;
		}

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (int i = 0; i < 4; ++i)
		{
			s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVBPtr->Color = color;
			s_Data.QuadVBPtr->TexCoord = texCoords[i];
			s_Data.QuadVBPtr->TexID = 0.0f;
			s_Data.QuadVBPtr->TilingFactor = 1.0f;
			s_Data.QuadVBPtr++;
		}

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		float texID = 0.0f;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == texture->GetBufferID())
			{
				texID = (float)i;
				break;
			}
		}

		if (texID == 0.0f)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = (float)s_Data.TexIndex;
			s_Data.TexSlots[s_Data.TexIndex++] = texture;
		}

		constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (int i = 0; i < 4; ++i)
		{
			s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVBPtr->Color = glm::vec4(1.0f);
			s_Data.QuadVBPtr->TexCoord = texCoords[i];
			s_Data.QuadVBPtr->TexID = texID;
			s_Data.QuadVBPtr->TilingFactor = tilingFactor;
			s_Data.QuadVBPtr++;
		}

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVBPtr->Color = color;
		s_Data.QuadVBPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVBPtr->TexID = 0.0f;
		s_Data.QuadVBPtr->TilingFactor = 1.0f;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVBPtr->Color = color;
		s_Data.QuadVBPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVBPtr->TexID = 0.0f;
		s_Data.QuadVBPtr->TilingFactor = 1.0f;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVBPtr->Color = color;
		s_Data.QuadVBPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVBPtr->TexID = 0.0f;
		s_Data.QuadVBPtr->TilingFactor = 1.0f;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVBPtr->Color = color;
		s_Data.QuadVBPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVBPtr->TexID = 0.0f;
		s_Data.QuadVBPtr->TilingFactor = 1.0f;
		s_Data.QuadVBPtr++;

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
	{
		DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, rotation, texture, tilingFactor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		float texID = 0.0f;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == texture->GetBufferID())
			{
				texID = (float)i;
				break;
			}
		}

		if (texID == 0.0f)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = (float)s_Data.TexIndex;
			s_Data.TexSlots[s_Data.TexIndex++] = texture;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVBPtr->Color = glm::vec4(1.0f);
		s_Data.QuadVBPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVBPtr->TexID = texID;
		s_Data.QuadVBPtr->TilingFactor = tilingFactor;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVBPtr->Color = glm::vec4(1.0f);
		s_Data.QuadVBPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVBPtr->TexID = texID;
		s_Data.QuadVBPtr->TilingFactor = tilingFactor;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVBPtr->Color = glm::vec4(1.0f);
		s_Data.QuadVBPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVBPtr->TexID = texID;
		s_Data.QuadVBPtr->TilingFactor = tilingFactor;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVBPtr->Color = glm::vec4(1.0f);
		s_Data.QuadVBPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVBPtr->TexID = texID;
		s_Data.QuadVBPtr->TilingFactor = tilingFactor;
		s_Data.QuadVBPtr++;

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor)
	{
		DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, rotation, subTexture, tilingFactor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		float texID = 0.0f;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == subTexture->GetTexture()->GetBufferID())
			{
				texID = (float)i;
				break;
			}
		}

		if (texID == 0.0f)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = (float)s_Data.TexIndex;
			s_Data.TexSlots[s_Data.TexIndex++] = subTexture->GetTexture();
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		const glm::vec2* texCoords = subTexture->GetTexCoords();

		for (int i = 0; i < 4; ++i)
		{
			s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVBPtr->Color = glm::vec4(1.0f);
			s_Data.QuadVBPtr->TexCoord = texCoords[i];
			s_Data.QuadVBPtr->TexID = texID;
			s_Data.QuadVBPtr->TilingFactor = tilingFactor;
			s_Data.QuadVBPtr++;
		}

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	void Renderer2D::StartNewBatch()
	{
		EndScene();

		s_Data.QuadIndexCnt = 0;
		s_Data.QuadVBPtr = s_Data.QuadVBBase;

		s_Data.TexIndex = 1;
	}

}
