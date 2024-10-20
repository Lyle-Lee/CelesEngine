#pragma once

#include "Camera.h"
#include "Renderer.h"
#include "EditorCamera.h"
#include "Celes/Scene/EntityComponent.h"

namespace Celes {

	class Renderer2D
	{
	public:
		CE_API static void Init();
		CE_API static void Shutdown();

		CE_API static void BeginScene(const Camera& camera, const glm::mat4& transform);
		CE_API static void BeginScene(const EditorCamera& camera);
		CE_API static void BeginScene(const OrthoCamera& camera); // TODO: remove
		CE_API static void EndScene();

		// Primitives
		CE_API static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		CE_API static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		
		CE_API static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		CE_API static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);

		CE_API static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);
		CE_API static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);

		CE_API static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		CE_API static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, int entityID = -1);

		CE_API static void DrawSprite(const glm::mat4& transform, SpriteRenderComponent& src, int entityID);

		// Rotation is in radians
		CE_API static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color);
		CE_API static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color);

		CE_API static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		CE_API static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);

		CE_API static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);
		CE_API static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);

		// Circles
		CE_API static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
		CE_API static void DrawCircle(const glm::mat4& transform, CircleRenderComponent& src, int entityID);

		// Lines
		CE_API static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);
		
		CE_API static void DrawRect(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		CE_API static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		//static void GetLineWidth();
		CE_API static void SetLineWidth(float width);

		struct CE_API Statistics
		{
			uint32_t DrawCallsCnt = 0;
			uint32_t QuadCnt = 0;

			uint32_t GetTotalVertexCnt() { return QuadCnt * 4; }
			uint32_t GetTotalIndexCnt() { return QuadCnt * 6; }
		};
		CE_API static Statistics GetStats();
		CE_API static void ResetStats();
	private:
		static void StartNewBatch();
		static void Flush();

		static Scope<RenderCommand> s_Cmd;
	};

}
