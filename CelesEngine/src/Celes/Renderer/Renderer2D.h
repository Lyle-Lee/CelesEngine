#pragma once

#include "Camera.h"
#include "Renderer.h"

namespace Celes {

	class CE_API Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthoCamera& camera);
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);

		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);

		// Rotation is in radians
		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color);

		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);

		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f);

		struct Statistics
		{
			uint32_t DrawCallsCnt = 0;
			uint32_t QuadCnt = 0;

			uint32_t GetTotalVertexCnt() { return QuadCnt * 4; }
			uint32_t GetTotalIndexCnt() { return QuadCnt * 6; }
		};
		static Statistics GetStats();
		static void ResetStats();
	private:
		static void StartNewBatch();

		static Scope<RenderCommand> s_Cmd;
	};

}
