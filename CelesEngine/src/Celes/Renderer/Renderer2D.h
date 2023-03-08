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

		// Primitives
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);

		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const glm::vec4& color);

		static void DrawRotatedQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		static void DrawRotatedQuad(const glm::vec3& pos, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
	private:
		static Scope<RenderCommand> s_Cmd;
	};

}
