#pragma once

// Platform detection
#ifdef _WIN32
#ifdef _WIN64
#define CE_PLATFORM_WINDOWS
#else
#error "x86 builds are not supported!"
#endif // _WIN64
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define CE_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define CE_PLATFORM_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif // TARGET_IPHONE_SIMULATOR
#elif defined(__ANDROID__)
#define CE_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define CE_PLATFORM_LINUX
#error "Linux is not supported!"
#else
#error "Unknown platform!"
#endif // _WIN32

#define CE_DYNAMIC_LINK 1 // Set to 0 if link statically

#ifdef CE_PLATFORM_WINDOWS
#if CE_DYNAMIC_LINK
#ifdef CE_BUILD_DLL
#define CE_API __declspec(dllexport)
#define IMGUI_API __declspec(dllexport)
#define yaml_cpp_EXPORTS
#else
#define CE_API __declspec(dllimport)
#define IMGUI_API __declspec(dllimport)
#endif // CE_BUILD_DLL
#else
#define CE_API
#endif // CE_DYNAMIC_LINK
#else
#error Celes only supports Windows currently!
#endif // CE_PLATFORM_WINDOWS

#ifdef CE_DEBUG
#if defined(CE_PLATFORM_WINDOWS)
#define CE_DEBUGBREAK() __debugbreak()
#elif defined(CE_PLATFORM_LINUX)
#include <signal.h>
#define CE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform does not support debugbreak yet!"
#endif
#define CE_ENABLE_ASSERTS
#else
#define CE_DEBUGBREAK()
#endif // CE_DEBUG

#ifdef  CE_ENABLE_ASSERTS
#define CE_ASSERT(x, ...) { if (!(x)) { CE_ERROR("Assertion Failed: {0}", __VA_ARGS__); CE_DEBUGBREAK(); } }
#define CE_CORE_ASSERT(x, ...) { if (!(x)) { CE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); CE_DEBUGBREAK(); } }
#else
#define CE_ASSERT(x, ...)
#define CE_CORE_ASSERT(x, ...)
#endif //  CE_ENABLE_ASSERTS

#define STB_IMAGE_IMPLEMENTATION

#include <memory>

namespace Celes {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
