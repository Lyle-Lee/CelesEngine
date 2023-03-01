#pragma once

#include "Log.h"

#ifdef CE_PLATFORM_WINDOWS
extern Celes::Application* Celes::CreateApp();

int main(int argc, char** argv)
{
	Celes::Log::Init();
	CE_CORE_WARN("Wellcome to Celes Engine!");
	CE_INFO("APP initialized!");

	auto app = Celes::CreateApp();
	app->Run();
	delete app;
}
#endif // CE_PLATFORM_WINDOWS
