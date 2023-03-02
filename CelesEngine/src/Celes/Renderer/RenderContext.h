#pragma once

#include "Celes/Core/Core.h"

namespace Celes {

	class RenderContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}
