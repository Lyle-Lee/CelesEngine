#include "PCH.h"
#include "Application.h"
#include "Log.h"
#include "Input.h"
#include "Celes/Renderer/Renderer.h"
#include "Celes/Utils/PlatformUtils.h"

namespace Celes {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, AppCommandLineArgs args) : m_CommandLineArgs(args)
	{
		CE_CORE_ASSERT(!s_Instance, "Application aleady exists!")
		s_Instance = this;

		m_Window = Window::Create(name);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_GUILayer = new GUILayer();
		PushOverlay(m_GUILayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			float time = Platform::GetTime();
			Timestep deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
				for (Layer* layer : m_LayerStk) layer->OnUpdate(deltaTime);

			m_GUILayer->Begin();
			for (Layer* layer : m_LayerStk) layer->OnGUIRender();
			m_GUILayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		//CE_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStk.end(); it != m_LayerStk.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled()) break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStk.Push(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStk.PushOverlay(layer);
		layer->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_IsRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}
