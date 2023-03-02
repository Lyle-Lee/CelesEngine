#include "PCH.h"
#include "OpenGLContext.h"

namespace Celes {

	OpenGLContext::OpenGLContext(GLFWwindow* window): m_WindowHandle(window)
	{
		CE_CORE_ASSERT(window, "Window is null!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		bool status = glewInit() == GLEW_OK;
		CE_CORE_ASSERT(status, "Failed to initialize GLEW!")
		CE_CORE_INFO("OpenGL Version: {0}", (char*)glGetString(GL_VERSION));
		CE_CORE_INFO("Device: {0}", (char*)glGetString(GL_RENDERER));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}