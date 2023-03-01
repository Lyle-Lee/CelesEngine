#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "LayerStack.h"
#include "GUI/GUILayer.h"
#include "Core/Timestep.h"

namespace Celes {

	class CE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		GUILayer* m_GUILayer;
		bool m_IsRunning = true;
		LayerStack m_LayerStk;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

	// Defined in client
	Application* CreateApp();
}

