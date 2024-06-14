#pragma once

#include "Celes/Renderer/Renderer.h"

namespace Celes {

	class OpenGLCommand : public RenderCommand
	{
	public:
		void Init() override;
		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;
		void SetViewport(int x, int y, int width, int height) override;

		void SetLineWidth(float width) override;

		void DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCnt = 0) override;
		void DrawLines(const Ref<VertexArray>& va, uint32_t vertexCnt) override;
	};

}
