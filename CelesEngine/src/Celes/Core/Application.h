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

	struct CE_API AppSpecification
	{
		std::string Name = "Celes Application";
		std::string WorkingDirectory;
		AppCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		CE_API Application(const AppSpecification& specification);
		CE_API virtual ~Application();

		CE_API void Run();

		CE_API void OnEvent(Event& e);
		CE_API void PushLayer(Layer* layer);
		CE_API void PushOverlay(Layer* layer);

		CE_API inline const AppSpecification& GetSpecification() const { return m_Specification; }
		CE_API inline Window& GetWindow() { return *m_Window; }
		CE_API inline static Application& Get() { return *s_Instance; }
		CE_API inline GUILayer* GetGUILayer() { return m_GUILayer; }

		CE_API void Close() { m_IsRunning = false; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		AppSpecification m_Specification;
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

