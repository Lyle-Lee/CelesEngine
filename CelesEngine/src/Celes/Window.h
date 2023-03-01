#pragma once

#include <PCH.h>
#include "Core.h"
#include "Events/Event.h"

namespace Celes {

	struct WindowInfo
	{
		std::string Title;
		unsigned int Width, Height;

		WindowInfo(const std::string& title = "Celes Engine", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	// Interface representing a desktop system based window
	class CE_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowInfo& info = WindowInfo());
		virtual void* GetNativeWindow() const = 0;
	};

}
