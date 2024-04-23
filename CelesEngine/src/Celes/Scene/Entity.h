#pragma once

#include "Scene.h"
#include "EntityComponent.h"
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

			T& compo = m_Scene->m_Registry.emplace<T>(m_EntityObject, std::forward<Args>(args)...);
			m_Scene->OnComponentAdd<T>(*this, compo);
			return compo;
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

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }

		operator bool() const { return m_EntityObject != entt::null; }
		operator entt::entity() const { return m_EntityObject; }
		operator uint32_t() const { return (uint32_t)m_EntityObject; }
		bool operator==(const Entity& other) const { return m_EntityObject == other.m_EntityObject && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
	private:
		entt::entity m_EntityObject = entt::null;
		Scene* m_Scene = nullptr;
	};

	class CE_API ScriptableEntity
	{
		friend class Scene;
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep dTime) {}
	private:
		Entity m_Entity;
	};

}
