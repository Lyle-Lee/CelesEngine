#pragma once

#include "Celes/Core/Timestep.h"
#include <entt.hpp>

namespace Celes {

	class Entity;
	class SceneHierarchyPanel;

	class CE_API Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep dTime);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};

}
