#pragma once

#include "Celes/Core/Core.h"
#include <chrono>
#include <functional>

namespace Celes {

	class Timer
	{
	public:
		CE_API Timer(const char* name, std::function<void(float)> func = nullptr) : m_Name(name), m_Stopped(false), m_Func(func)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		CE_API ~Timer()
		{
			if (!m_Stopped) Stop();
		}

		CE_API void Stop();
	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
		std::function<void(float)> m_Func;
	};

}
