#pragma once

#include <string>
#include "Celes/Core/Core.h"

namespace Celes {

	class CE_API FileDialogs
	{
	public:
		// Both return empty string if canceled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

	class CE_API Platform
	{
	public:
		static float GetTime();
	};

}
