#pragma once

#include <Celes/Renderer/RenderContext.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Celes {

	class OpenGLContext : public RenderContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		void Init() override;
		void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};

}
