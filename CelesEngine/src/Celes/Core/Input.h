#pragma once

#include "Core.h"

namespace Celes {

	class CE_API Input
	{
	public:
		static bool IsKeyPressed(int keyCode);

		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};

}
