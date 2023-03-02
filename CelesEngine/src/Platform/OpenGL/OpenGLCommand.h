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
		void DrawIndexed(const Ref<VertexArray>& va) override;
	};

}
