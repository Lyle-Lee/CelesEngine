#include <Celes.h>
#include <Celes/Core/EntryPoint.h>
#include "EditorLayer.h"

namespace Celes {

	class Glamoth : public Application
	{
	public:
		Glamoth(AppCommandLineArgs args) : Application("Glamoth", args)
		{
			PushLayer(new EditorLayer());
		}

		~Glamoth() {}
	};

	Application* CreateApp(AppCommandLineArgs args)
	{
		return new Glamoth(args);
	}

}
