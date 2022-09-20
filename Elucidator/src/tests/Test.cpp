#include "Test.h"
#include "../ImGui/imgui.h"

test::TestMenu::TestMenu(Test*& testPtr)
	: m_CurrentTest(testPtr)
{
}

void test::TestMenu::onImGuiRender()
{
	for (auto& test : m_Tests)
	{
		if (ImGui::Button(test.first.c_str()))
			m_CurrentTest = test.second();
	}
}
