#pragma once

#include "Celes/Core/Timestep.h"
#include <entt.hpp>

namespace Celes {

	class Entity;
	class SceneHierarchyPanel;
	struct CameraComponent;
	struct SpriteRenderComponent;
	struct NativeScriptComponent;

	class CE_API Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdate(Timestep dTime);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		template<typename T>
		void OnComponentAdd(Entity entity, T& component);

		template<>
		void OnComponentAdd<CameraComponent>(Entity entity, CameraComponent& component);

		template<>
		void OnComponentAdd<SpriteRenderComponent>(Entity entity, SpriteRenderComponent& component);

		template<>
		void OnComponentAdd<NativeScriptComponent>(Entity entity, NativeScriptComponent& component);

		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};

}
