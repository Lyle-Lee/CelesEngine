#include "TestClearColor.h"
#include "../Utils.h"
#include "../ImGui/imgui.h"

test::TestClearColor::TestClearColor()
	: m_ClearColor{0.0f, 0.5f, 1.0f, 1.0f}
{

}

test::TestClearColor::~TestClearColor()
{
}

void test::TestClearColor::onUpdate(float deltaTime)
{
}

void test::TestClearColor::onRender()
{
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void test::TestClearColor::onImGuiRender()
{
	ImGui::ColorEdit4("Clear Color", m_ClearColor);
}
