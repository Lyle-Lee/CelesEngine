#include <Celes.h>
#include <Celes/Core/EntryPoint.h>
#include "EditorLayer.h"

namespace Celes {

	class CelesEditor : public Application
	{
	public:
		CelesEditor(): Application("Celes Editor")
		{
			PushLayer(new EditorLayer());
		}

		~CelesEditor() {}
	};

	Application* CreateApp()
	{
		return new CelesEditor();
	}

}
