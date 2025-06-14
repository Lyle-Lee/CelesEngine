#include "PCH.h"
#include "MacOSWindow.h"
#include "Celes/Events/ApplicationEvent.h"
#include "Celes/Events/KeyEvent.h"
#include "Celes/Events/MouseEvent.h"

namespace Celes {

	static bool s_GLFWInitialized = false;

	MacOSWindow::MacOSWindow(const WindowInfo& info)
	{
		Init(info);
	}

	MacOSWindow::~MacOSWindow()
	{
		Shutdown();
	}

	void MacOSWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void MacOSWindow::SetVSync(bool enabled)
	{
		if (enabled) glfwSwapInterval(1);
		else glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool MacOSWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void MacOSWindow::Init(const WindowInfo& info)
	{
		m_Data.Title = info.Title;
		m_Data.Width = info.Width;
		m_Data.Height = info.Height;

		CE_CORE_INFO("Creating window {0} ({1}, {2})", info.Title, info.Width, info.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			CE_CORE_ASSERT(success, "Failed to initialize GLFW!");

			glfwSetErrorCallback([](int error, const char* description)
			{
				CE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
			});

			s_GLFWInitialized = true;
		}

		// macOS supports OpenGL up to version 4.1
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_Window = glfwCreateWindow((int)info.Width, (int)info.Height, m_Data.Title.c_str(), nullptr, nullptr);
		
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scanCode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressEvent event(key, false);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressEvent event(key, true);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int key)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypeEvent event(key);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleaseEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrollEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMoveEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void MacOSWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

}
