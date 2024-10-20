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
		int TexID;
		float TilingFactor;

		// Editor only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 Position;
		float Thickness;
		glm::vec3 LocalPosition;
		float Fade;
		glm::vec4 Color;

		// Editor only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor only
		int EntityID;
	};

	struct Renderer2DStorage
	{
		static const uint32_t MaxQuadsCnt = 10000;
		static const uint32_t MaxVerticesCnt = MaxQuadsCnt * 4;
		static const uint32_t MaxIndicesCnt = MaxQuadsCnt * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		//Ref<Shader> FlatColorShader;
		Ref<Shader> QuadShader;
		Ref<Texture> BlankTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t QuadIndexCnt = 0;
		QuadVertex* QuadVBBase = nullptr;
		QuadVertex* QuadVBPtr = nullptr;

		uint32_t CircleIndexCnt = 0;
		CircleVertex* CircleVBBase = nullptr;
		CircleVertex* CircleVBPtr = nullptr;

		uint32_t LineVertexCnt = 0;
		LineVertex* LineVBBase = nullptr;
		LineVertex* LineVBPtr = nullptr;
		float LineWidth = 2.0f;

		std::array<Ref<Texture>, MaxTextureSlots> TexSlots;
		uint32_t TexIndex = 1; // 0 = blank texture

		glm::vec4 QuadVertexPositions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f }, // x, y, z, w
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f } };

		struct CameraData
		{
			glm::mat4 ViewProjection = glm::mat4(1.0f);
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		Renderer2D::Statistics Stats;
	};

	static Renderer2DStorage s_Data;
	Scope<RenderCommand> Renderer2D::s_Cmd = CreateScope<OpenGLCommand>();

	void Renderer2D::Init()
	{
		//s_Data = new Renderer2DStorage();

		// Quads
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticesCnt * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ "aPosition",     ShaderDataType::Float3 },
			{ "aColor",	       ShaderDataType::Float4 },
			{ "aTexCoord",     ShaderDataType::Float2 },
			{ "aTexID",	       ShaderDataType::Int },
			{ "aTilingFactor", ShaderDataType::Float },
			{ "aEntityID",     ShaderDataType::Int }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVBBase = new QuadVertex[s_Data.MaxVerticesCnt];

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

		// Circles
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticesCnt * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ "aPosition",      ShaderDataType::Float3 },
			{ "aThickness",     ShaderDataType::Float },
			{ "aLocalPosition", ShaderDataType::Float3 },
			{ "aFade",          ShaderDataType::Float },
			{ "aColor",         ShaderDataType::Float4 },
			{ "aEntityID",      ShaderDataType::Int }
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);

		s_Data.CircleVBBase = new CircleVertex[s_Data.MaxVerticesCnt];

		s_Data.CircleVertexArray->SetIndexBuffer(indexBuffer); // Use the same index buffer since geometry is identical.

		// Lines
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVerticesCnt * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ "aPosition", ShaderDataType::Float3 },
			{ "aColor",    ShaderDataType::Float4 },
			{ "aEntityID", ShaderDataType::Int }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

		s_Data.LineVBBase = new LineVertex[s_Data.MaxVerticesCnt];

		s_Data.BlankTexture = Texture2D::Create(1, 1);
		uint32_t blankData = 0xffffffff;
		s_Data.BlankTexture->SetData(&blankData, sizeof(uint32_t));

		//int samplers[s_Data.MaxTextureSlots];
		//for (int i = 0; i < s_Data.MaxTextureSlots; ++i) samplers[i] = i;

		//s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.vert.glsl", "assets/shaders/FlatColor.frag.glsl", true);
		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.vert.glsl", "assets/shaders/Renderer2D_Quad.frag.glsl", true);
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.vert.glsl", "assets/shaders/Renderer2D_Circle.frag.glsl", true);
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.vert.glsl", "assets/shaders/Renderer2D_Line.frag.glsl", true);
		//s_Data.QuadShader->Bind();
		//s_Data.QuadShader->SetIntArray("uTextures", samplers, s_Data.MaxTextureSlots);

		s_Data.TexSlots[0] = s_Data.BlankTexture;
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DStorage::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		//delete s_Data;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));

		s_Data.QuadIndexCnt = 0;
		s_Data.QuadVBPtr = s_Data.QuadVBBase;
		s_Data.CircleIndexCnt = 0;
		s_Data.CircleVBPtr = s_Data.CircleVBBase;
		s_Data.LineVertexCnt = 0;
		s_Data.LineVBPtr = s_Data.LineVBBase;

		s_Data.TexIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetVP();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));

		s_Data.QuadIndexCnt = 0;
		s_Data.QuadVBPtr = s_Data.QuadVBBase;
		s_Data.CircleIndexCnt = 0;
		s_Data.CircleVBPtr = s_Data.CircleVBBase;
		s_Data.LineVertexCnt = 0;
		s_Data.LineVBPtr = s_Data.LineVBBase;

		s_Data.TexIndex = 1;
	}

	void Renderer2D::BeginScene(const OrthoCamera& camera)
	{
		//s_Data.FlatColorShader->Bind();
		//s_Data.FlatColorShader->SetMat4("uViewProj", camera.GetVP());

		s_Data.CameraBuffer.ViewProjection = camera.GetVP();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DStorage::CameraData));

		s_Data.QuadIndexCnt = 0;
		s_Data.QuadVBPtr = s_Data.QuadVBBase;
		s_Data.CircleIndexCnt = 0;
		s_Data.CircleVBPtr = s_Data.CircleVBBase;
		s_Data.LineVertexCnt = 0;
		s_Data.LineVBPtr = s_Data.LineVBBase;

		s_Data.TexIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCnt)
		{
			uint32_t dataSize = (uint8_t*)s_Data.QuadVBPtr - (uint8_t*)s_Data.QuadVBBase;
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVBBase, dataSize);

			for (uint32_t i = 0; i < s_Data.TexIndex; ++i)
				s_Data.TexSlots[i]->Bind(i);

			s_Data.QuadShader->Bind();
			s_Cmd->DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCnt);

			s_Data.Stats.DrawCallsCnt++;
		}

		if (s_Data.CircleIndexCnt)
		{
			uint32_t dataSize = (uint8_t*)s_Data.CircleVBPtr - (uint8_t*)s_Data.CircleVBBase;
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVBBase, dataSize);

			s_Data.CircleShader->Bind();
			s_Cmd->DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCnt);

			s_Data.Stats.DrawCallsCnt++;
		}

		if (s_Data.LineVertexCnt)
		{
			uint32_t dataSize = (uint8_t*)s_Data.LineVBPtr - (uint8_t*)s_Data.LineVBBase;
			s_Data.LineVertexBuffer->SetData(s_Data.LineVBBase, dataSize);

			s_Data.LineShader->Bind();
			s_Cmd->SetLineWidth(s_Data.LineWidth);
			s_Cmd->DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCnt);

			s_Data.Stats.DrawCallsCnt++;
		}
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

		uint32_t texID = 0;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == subTexture->GetTexture()->GetBufferID())
			{
				texID = i;
				break;
			}
		}

		if (texID == 0)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = s_Data.TexIndex;
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

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		constexpr glm::vec2 texCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (int i = 0; i < 4; ++i)
		{
			s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVBPtr->Color = color;
			s_Data.QuadVBPtr->TexCoord = texCoords[i];
			s_Data.QuadVBPtr->TexID = 0;
			s_Data.QuadVBPtr->TilingFactor = 1.0f;
			s_Data.QuadVBPtr->EntityID = entityID;
			s_Data.QuadVBPtr++;
		}

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, int entityID)
	{
		if (s_Data.QuadIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
			StartNewBatch();

		uint32_t texID = 0;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == texture->GetBufferID())
			{
				texID = i;
				break;
			}
		}

		if (texID == 0)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = s_Data.TexIndex;
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
			s_Data.QuadVBPtr->EntityID = entityID;
			s_Data.QuadVBPtr++;
		}

		s_Data.QuadIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRenderComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
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
		s_Data.QuadVBPtr->TexID = 0;
		s_Data.QuadVBPtr->TilingFactor = 1.0f;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVBPtr->Color = color;
		s_Data.QuadVBPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVBPtr->TexID = 0;
		s_Data.QuadVBPtr->TilingFactor = 1.0f;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVBPtr->Color = color;
		s_Data.QuadVBPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVBPtr->TexID = 0;
		s_Data.QuadVBPtr->TilingFactor = 1.0f;
		s_Data.QuadVBPtr++;

		s_Data.QuadVBPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVBPtr->Color = color;
		s_Data.QuadVBPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVBPtr->TexID = 0;
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

		uint32_t texID = 0;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == texture->GetBufferID())
			{
				texID = i;
				break;
			}
		}

		if (texID == 0)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = s_Data.TexIndex;
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

		uint32_t texID = 0;
		for (uint32_t i = 1; i < s_Data.TexIndex; ++i)
		{
			if (s_Data.TexSlots[i]->GetBufferID() == subTexture->GetTexture()->GetBufferID())
			{
				texID = i;
				break;
			}
		}

		if (texID == 0)
		{
			if (s_Data.TexIndex >= Renderer2DStorage::MaxTextureSlots)
				StartNewBatch();

			texID = s_Data.TexIndex;
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

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		// TODO: implement for circles
		//if (s_Data.CircleIndexCnt >= Renderer2DStorage::MaxIndicesCnt)
		//	StartNewBatch();

		for (int i = 0; i < 4; ++i)
		{
			s_Data.CircleVBPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVBPtr->Thickness = thickness;
			s_Data.CircleVBPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVBPtr->Fade = fade;
			s_Data.CircleVBPtr->Color = color;
			s_Data.CircleVBPtr->EntityID = entityID;
			s_Data.CircleVBPtr++;
		}

		s_Data.CircleIndexCnt += 6;

		s_Data.Stats.QuadCnt++;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, CircleRenderComponent& src, int entityID)
	{
		DrawCircle(transform, src.Color, src.Thickness, src.Fade, entityID);
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_Data.LineVBPtr->Position = p0;
		s_Data.LineVBPtr->Color = color;
		s_Data.LineVBPtr->EntityID = entityID;
		s_Data.LineVBPtr++;

		s_Data.LineVBPtr->Position = p1;
		s_Data.LineVBPtr->Color = color;
		s_Data.LineVBPtr->EntityID = entityID;
		s_Data.LineVBPtr++;

		s_Data.LineVertexCnt += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f, pos.z);
		glm::vec3 p1 = glm::vec3(pos.x + size.x * 0.5f, pos.y - size.y * 0.5f, pos.z);
		glm::vec3 p2 = glm::vec3(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f, pos.z);
		glm::vec3 p3 = glm::vec3(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f, pos.z);
	
		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (int i = 0; i < 4; ++i)
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

		for (int i = 0; i < 4; ++i)
			DrawLine(lineVertices[i], lineVertices[(i + 1) % 4], color, entityID);
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
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

		s_Data.CircleIndexCnt = 0;
		s_Data.CircleVBPtr = s_Data.CircleVBBase;

		s_Data.TexIndex = 1;
	}

}
