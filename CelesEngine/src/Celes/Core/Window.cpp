#include "PCH.h"
#include "Window.h"

#ifdef CE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#elif defined(CE_PLATFORM_MACOS)
#include "Platform/macOS/MacOSWindow.h"
#endif

namespace Celes {

	Scope<Window> Window::Create(const WindowInfo& info)
	{
#ifdef CE_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(info);
#elif defined(CE_PLATFORM_MACOS)
		return CreateScope<MacOSWindow>(info);
#else
		CE_CORE_ASSERT(false, "Unknown platform!")
		return nullptr;
#endif // CE_PLATFORM_WINDOWS
	}

}
