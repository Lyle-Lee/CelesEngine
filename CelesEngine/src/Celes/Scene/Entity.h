#pragma once

#include "Scene.h"
#include <entt.hpp>

namespace Celes {

	class CE_API Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity object, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent() { return m_Scene->m_Registry.any_of<T>(m_EntityObject); }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			CE_CORE_ASSERT(!HasComponent<T>(), "Entity already has the component!")

			return m_Scene->m_Registry.emplace<T>(m_EntityObject, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			CE_CORE_ASSERT(HasComponent<T>(), "Entity does not have the component!")

			return m_Scene->m_Registry.get<T>(m_EntityObject);
		}

		template<typename T>
		void RemoveComponent()
		{
			CE_CORE_ASSERT(HasComponent<T>(), "Entity does not have the component!")

			m_Scene->m_Registry.remove<T>(m_EntityObject);
		}

		operator bool() const { return m_EntityObject != entt::null; }
	private:
		entt::entity m_EntityObject = entt::null;
		Scene* m_Scene = nullptr;
	};

}
