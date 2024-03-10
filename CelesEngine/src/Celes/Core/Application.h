#pragma once

#include "Core.h"
#include "Window.h"
#include "Celes/Events/Event.h"
#include "Celes/Events/ApplicationEvent.h"
#include "LayerStack.h"
#include "Celes/GUI/GUILayer.h"
#include "Timestep.h"

namespace Celes {

	struct CE_API AppCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int idx) const
		{
			CE_CORE_ASSERT(idx < Count, "Argument index overflowed!")
			return Args[idx];
		}
	};

	class CE_API Application
	{
	public:
		Application(const std::string& name = "Celes App", AppCommandLineArgs args = AppCommandLineArgs());
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline AppCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
		inline GUILayer* GetGUILayer() { return m_GUILayer; }

		void Close() { m_IsRunning = false; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		AppCommandLineArgs m_CommandLineArgs;
		Scope<Window> m_Window;
		GUILayer* m_GUILayer;
		bool m_IsRunning = true;
		bool m_Minimized = false;
		LayerStack m_LayerStk;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

	// Defined in client
	Application* CreateApp(AppCommandLineArgs args);
}

