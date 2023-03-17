#include "PCH.h"
#include "Scene.h"
#include "Entity.h"
#include "EntityComponent.h"
#include "Celes/Renderer/Renderer2D.h"
#include <glm.hpp>

namespace Celes {

	static void Calculate(const glm::mat4& transform)
	{

	}

	Scene::Scene()
	{
#if 0
		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		TransformComponent& transformCompo = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}
#endif
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tagCompo = entity.AddComponent<TagComponent>();
		tagCompo.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::OnUpdate(Timestep dTime)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
		for (auto& entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRenderComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}

}
