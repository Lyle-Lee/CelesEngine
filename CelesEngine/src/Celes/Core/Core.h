#pragma once

#define CE_DYNAMIC_LINK 1 // Set to 0 if link statically

#ifdef CE_PLATFORM_WINDOWS
#if CE_DYNAMIC_LINK
#ifdef CE_BUILD_DLL
#define CE_API __declspec(dllexport)
#define IMGUI_API __declspec(dllexport)
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
#define CE_ENABLE_ASSERTS
#endif // CE_DEBUG

#ifdef  CE_ENABLE_ASSERTS
#define CE_ASSERT(x, ...) { if (!(x)) { CE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define CE_CORE_ASSERT(x, ...) { if (!(x)) { CE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define CE_ASSERT(x, ...)
#define CE_CORE_ASSERT(x, ...)
#endif //  CE_ENABLE_ASSERTS

#define STB_IMAGE_IMPLEMENTATION

#include <memory>

namespace Celes {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}
