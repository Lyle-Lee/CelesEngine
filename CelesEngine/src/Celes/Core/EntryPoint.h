#pragma once

#include "Log.h"
#include "Application.h"

#if defined(CE_PLATFORM_WINDOWS) || defined(CE_PLATFORM_MACOS)
extern Celes::Application* Celes::CreateApp(AppCommandLineArgs args);

int main(int argc, char** argv)
{
	Celes::Log::Init();
	CE_CORE_WARN("Wellcome to Celes Engine!");

	auto app = Celes::CreateApp({ argc, argv });
	CE_INFO("APP initialized!");
	app->Run();
	delete app;
}
#endif // CE_PLATFORM
