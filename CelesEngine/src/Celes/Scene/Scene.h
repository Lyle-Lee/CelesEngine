#pragma once

#include "Celes/Core/Timestep.h"
#include <entt.hpp>

namespace Celes {

	class Entity;

	class CE_API Scene
	{
		friend class Entity;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep dTime);
	private:
		entt::registry m_Registry;
	};

}
