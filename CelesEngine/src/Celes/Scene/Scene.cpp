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
		//m_Registry.on_construct<CameraComponent>().connect<&OnComponentAdd<CameraComponent>>();
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

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
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
		glm::mat4 cameraTransform;
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto& entity : view)
		{
			auto [transformCompo, cameraCompo] = view.get<TransformComponent, CameraComponent>(entity);

			if (cameraCompo.Primary)
			{
				mainCamera = &cameraCompo.Camera;
				cameraTransform = transformCompo.GetTransform();
				break;
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
			for (auto& entity : group)
			{
				auto [transformCompo, spriteCompo] = group.get<TransformComponent, SpriteRenderComponent>(entity);

				//Renderer2D::DrawQuad(transformCompo.GetTransform(), spriteCompo.Color);
				Renderer2D::DrawSprite(transformCompo.GetTransform(), spriteCompo, (int)entity);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(Timestep dTime, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
		for (auto& entity : group)
		{
			auto [transformCompo, spriteCompo] = group.get<TransformComponent, SpriteRenderComponent>(entity);

			//Renderer2D::DrawQuad(transformCompo.GetTransform(), spriteCompo.Color);
			Renderer2D::DrawSprite(transformCompo.GetTransform(), spriteCompo, (int)entity);
		}

		Renderer2D::EndScene();
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

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraCompo = view.get<CameraComponent>(entity);
			if (cameraCompo.Primary)
				return Entity(entity, this);
		}

		return {};
	}

	template<typename T>
	void Scene::OnComponentAdd(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdd<TransformComponent>(Entity entity, TransformComponent& component)
	{}

	template<>
	void Scene::OnComponentAdd<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdd<TagComponent>(Entity entity, TagComponent& component)
	{}

	template<>
	void Scene::OnComponentAdd<SpriteRenderComponent>(Entity entity, SpriteRenderComponent& component)
	{}

	template<>
	void Scene::OnComponentAdd<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{}

}
