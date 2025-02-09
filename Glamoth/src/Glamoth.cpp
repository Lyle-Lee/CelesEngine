#include <Celes.h>
#include <Celes/Core/EntryPoint.h>
#include "EditorLayer.h"

namespace Celes {

	class Glamoth : public Application
	{
	public:
		Glamoth(const AppSpecification& specification) : Application(specification)
		{
			PushLayer(new EditorLayer());
		}

		~Glamoth() {}
	};

	Application* CreateApp(AppCommandLineArgs args)
	{
		AppSpecification spec;
		spec.Name = "Glamoth";
		spec.CommandLineArgs = args;

		return new Glamoth(spec);
	}

}
