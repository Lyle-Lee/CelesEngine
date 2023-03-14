#pragma once

#include "PCH.h"
#include "Core.h"
#include "Celes/Events/Event.h"

namespace Celes {

	struct WindowInfo
	{
		std::string Title;
		uint32_t Width, Height;

		WindowInfo(const std::string& title = "Celes Engine", uint32_t width = 1280, uint32_t height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	// Interface representing a desktop system based window
	class CE_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		static Scope<Window> Create(const WindowInfo& info = WindowInfo());
		virtual void* GetNativeWindow() const = 0;
	};

}
