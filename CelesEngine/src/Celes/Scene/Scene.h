#pragma once

#include "Celes/Core/Timestep.h"
#include "Celes/Core/UUID.h"
#include "Celes/Renderer/EditorCamera.h"
#include <entt.hpp>

class b2World;

namespace Celes {

	class Entity;
	class SceneHierarchyPanel;
	class SceneSerializer;
	struct CameraComponent;
	struct SpriteRenderComponent;
	struct CircleRenderComponent;
	struct NativeScriptComponent;
	struct Rigidbody2DComponent;
	struct BoxCollider2DComponent;
	struct CircleCollider2DComponent;

	//template class CE_API entt::basic_registry<entt::entity, std::allocator<Entity>>;

	class CE_API Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> src);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void DuplicateEntity(Entity entity);

		// Update runtime
		void OnUpdate(Timestep dTime);
		void OnRuntimeStart();
		void OnRuntimeStop();

		// Update simulation
		void OnUpdateSimulation(Timestep dTime, EditorCamera& camera);
		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateEditor(Timestep dTime, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdd(Entity entity, T& component);

		template<>
		void OnComponentAdd<CameraComponent>(Entity entity, CameraComponent& component);

		template<>
		void OnComponentAdd<SpriteRenderComponent>(Entity entity, SpriteRenderComponent& component);

		template<>
		void OnComponentAdd<CircleRenderComponent>(Entity entity, CircleRenderComponent& component);

		template<>
		void OnComponentAdd<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component);

		template<>
		void OnComponentAdd<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component);

		template<>
		void OnComponentAdd<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void RenderInEditor(EditorCamera& camera);

		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		b2World* m_2DPhysicsWorld = nullptr;
	};

}
