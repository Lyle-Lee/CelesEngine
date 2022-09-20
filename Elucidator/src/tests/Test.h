#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>

namespace test
{
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void onUpdate(float deltaTime) {}
		virtual void onRender() {}
		virtual void onImGuiRender() {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& testPtr);

		void onImGuiRender() override;

		template<typename T>
		void registerTest(const std::string& name)
		{
			std::cout << "Registering test: " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}
	private:
		Test*& m_CurrentTest;
		// Test name with its constructor
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	};
}