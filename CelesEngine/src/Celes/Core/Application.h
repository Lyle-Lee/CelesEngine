#pragma once

#include "Core.h"
#include "Window.h"
#include "Celes/Events/Event.h"
#include "Celes/Events/ApplicationEvent.h"
#include "LayerStack.h"
#include "Celes/GUI/GUILayer.h"
#include "Timestep.h"

namespace Celes {

	class CE_API Application
	{
	public:
		Application(const std::string& name = "Celes App");
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

		void Close() { m_IsRunning = false; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Scope<Window> m_Window;
		GUILayer* m_GUILayer;
		bool m_IsRunning = true;
		bool m_Minimized = false;
		LayerStack m_LayerStk;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

	// Defined in client
	Application* CreateApp();
}

