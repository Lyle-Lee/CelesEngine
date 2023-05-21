#pragma once

#include <Celes.h>

namespace Celes {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnGUIRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}
