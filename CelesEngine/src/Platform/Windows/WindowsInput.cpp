#include "PCH.h"
#include "Celes/Core/Input.h"
#include "Celes/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Celes {

	bool Input::IsKeyPressed(int keyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int status = glfwGetKey(window, keyCode);
		return status == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int status = glfwGetMouseButton(window, button);
		return status == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return {(float)xPos, (float)yPos};
	}

	float Input::GetMouseX()
	{
		auto [x, _] = GetMousePos();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [_, y] = GetMousePos();
		return y;
	}

}