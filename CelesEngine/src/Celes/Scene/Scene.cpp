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
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity(entity, this);
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(dTime);
		});

		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto& entity : view)
		{
			auto [transformCompo, cameraCompo] = view.get<TransformComponent, CameraComponent>(entity);

			if (cameraCompo.Primary)
			{
				mainCamera = &cameraCompo.Camera;
				cameraTransform = &transformCompo.Transform;
				break;
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
			for (auto& entity : group)
			{
				auto [transformCompo, spriteCompo] = group.get<TransformComponent, SpriteRenderComponent>(entity);

				Renderer2D::DrawQuad(transformCompo, spriteCompo.Color);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto& entity : view)
		{
			auto& cameraCompo = view.get<CameraComponent>(entity);
			if (!cameraCompo.FixedAspectRatio)
				cameraCompo.Camera.SetViewportSize(width, height);
		}
	}

}
