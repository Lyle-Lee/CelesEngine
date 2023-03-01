#pragma once

#include "Core.h"
#include <spdlog.h>
#include <fmt/ostr.h>

namespace Celes {

	class CE_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

#define CE_CORE_TRACE(...) ::Celes::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CE_CORE_INFO(...) ::Celes::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CE_CORE_WARN(...) ::Celes::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CE_CORE_ERROR(...) ::Celes::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CE_CORE_FATAL(...) ::Celes::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define CE_TRACE(...) ::Celes::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CE_INFO(...)  ::Celes::Log::GetClientLogger()->info(__VA_ARGS__)
#define CE_WARN(...)  ::Celes::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CE_ERROR(...) ::Celes::Log::GetClientLogger()->error(__VA_ARGS__)
#define CE_FATAL(...) ::Celes::Log::GetClientLogger()->critical(__VA_ARGS__)
