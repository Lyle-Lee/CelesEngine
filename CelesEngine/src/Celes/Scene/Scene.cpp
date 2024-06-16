#include "PCH.h"
#include "Scene.h"
#include "Entity.h"
#include "EntityComponent.h"
#include "Celes/Renderer/Renderer2D.h"
#include <glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>

namespace Celes {

	static b2BodyType Rigidbody2DTypeToB2BodyType(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Celes::Rigidbody2DComponent::BodyType::Static:
			return b2BodyType::b2_staticBody;
		case Celes::Rigidbody2DComponent::BodyType::Dynamic:
			return b2BodyType::b2_dynamicBody;
		case Celes::Rigidbody2DComponent::BodyType::Kinematic:
			return b2BodyType::b2_kinematicBody;
		default:
			CE_CORE_ASSERT(false, "Unknown body type!")
			break;
		}

		return b2BodyType::b2_staticBody;
	}

	Scene::Scene()
	{
		//m_Registry.on_construct<CameraComponent>().connect<&OnComponentAdd<CameraComponent>>();
	}

	Scene::~Scene()
	{
	}

	template<typename T>
	static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto idView = srcRegistry.view<IDComponent>();

		// Copy the component to the target entity which has the same UUID.
		for (auto& entity : idView)
		{
			if (!srcRegistry.any_of<T>(entity))
				continue;

			UUID uuid = srcRegistry.get<IDComponent>(entity).ID;
			CE_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "Could not find the target entity during copy.")
			entt::entity dstEntity = enttMap.at(uuid);

			auto& component = srcRegistry.get<T>(entity);
			dstRegistry.emplace_or_replace<T>(dstEntity, component);
		}
	}

	template<typename T>
	static void CopyComponentIfExists(Entity dstEntity, Entity srcEntity)
	{
		if (srcEntity.HasComponent<T>())
			dstEntity.AddOrReplaceComponent<T>(srcEntity.GetComponent<T>());
	}

	Ref<Scene> Scene::Copy(Ref<Scene> src)
	{
		Ref<Scene> dst = CreateRef<Scene>();
		dst->m_ViewportWidth = src->m_ViewportWidth;
		dst->m_ViewportHeight = src->m_ViewportHeight;
		
		auto idView = src->m_Registry.view<IDComponent>();
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities for new scene.
		for (auto& entity : idView)
		{
			UUID uuid = src->m_Registry.get<IDComponent>(entity).ID;
			const auto& name = src->m_Registry.get<TagComponent>(entity).Tag;
			auto dstEntity = dst->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)dstEntity;
		}

		CopyComponent<TransformComponent>(dst->m_Registry, src->m_Registry, enttMap);
		CopyComponent<SpriteRenderComponent>(dst->m_Registry, src->m_Registry, enttMap);
		CopyComponent<CircleRenderComponent>(dst->m_Registry, src->m_Registry, enttMap);
		CopyComponent<CameraComponent>(dst->m_Registry, src->m_Registry, enttMap);
		CopyComponent<NativeScriptComponent>(dst->m_Registry, src->m_Registry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dst->m_Registry, src->m_Registry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dst->m_Registry, src->m_Registry, enttMap);
		CopyComponent<CircleCollider2DComponent>(dst->m_Registry, src->m_Registry, enttMap);

		return dst;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tagCompo = entity.AddComponent<TagComponent>();
		tagCompo.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRenderComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRenderComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
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

		// Physics
		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_2DPhysicsWorld->Step(dTime, velocityIterations, positionIterations);
		auto viewPhysic = m_Registry.view<TransformComponent, Rigidbody2DComponent>();
		for (auto& entity : viewPhysic)
		{
			auto [transformCompo, rb2dCompo] = viewPhysic.get<TransformComponent, Rigidbody2DComponent>(entity);

			b2Body* body = (b2Body*)rb2dCompo.RuntimeBody;
			const auto& pos = body->GetPosition();
			transformCompo.Translation.x = pos.x;
			transformCompo.Translation.y = pos.y;
			transformCompo.Rotation.z = body->GetAngle();
		}

		// Render
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

			// Draw sprites
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
				for (auto& entity : group)
				{
					auto [transformCompo, spriteCompo] = group.get<TransformComponent, SpriteRenderComponent>(entity);

					//Renderer2D::DrawQuad(transformCompo.GetTransform(), spriteCompo.Color);
					Renderer2D::DrawSprite(transformCompo.GetTransform(), spriteCompo, (int)entity);
				}
			}

			// Draw circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRenderComponent>();
				for (auto& entity : view)
				{
					auto [transformCompo, circleCompo] = view.get<TransformComponent, CircleRenderComponent>(entity);

					Renderer2D::DrawCircle(transformCompo.GetTransform(), circleCompo, (int)entity);
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnRuntimeStart()
	{
		m_2DPhysicsWorld = new b2World({ 0.0f, -9.8f });
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto& e : view)
		{
			Entity entity(e, this);
			auto& transformCompo = entity.GetComponent<TransformComponent>();
			auto& rb2dCompo = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToB2BodyType(rb2dCompo.Type);
			bodyDef.position.Set(transformCompo.Translation.x, transformCompo.Translation.y);
			bodyDef.angle = transformCompo.Rotation.z;

			b2Body* body = m_2DPhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2dCompo.FixedRotation);
			rb2dCompo.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2dCompo = entity.GetComponent<BoxCollider2DComponent>();
				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2dCompo.Size.x * transformCompo.Scale.x, bc2dCompo.Size.y * transformCompo.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2dCompo.Density;
				fixtureDef.friction = bc2dCompo.Friction;
				fixtureDef.restitution = bc2dCompo.Restitution;
				fixtureDef.restitutionThreshold = bc2dCompo.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2dCompo = entity.GetComponent<CircleCollider2DComponent>();
				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2dCompo.Offset.x, cc2dCompo.Offset.y);
				circleShape.m_radius = cc2dCompo.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2dCompo.Density;
				fixtureDef.friction = cc2dCompo.Friction;
				fixtureDef.restitution = cc2dCompo.Restitution;
				fixtureDef.restitutionThreshold = cc2dCompo.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_2DPhysicsWorld;
		m_2DPhysicsWorld = nullptr;
	}

	void Scene::OnUpdateEditor(Timestep dTime, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw sprites
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);
			for (auto& entity : group)
			{
				auto [transformCompo, spriteCompo] = group.get<TransformComponent, SpriteRenderComponent>(entity);

				//Renderer2D::DrawQuad(transformCompo.GetTransform(), spriteCompo.Color);
				Renderer2D::DrawSprite(transformCompo.GetTransform(), spriteCompo, (int)entity);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRenderComponent>();
			for (auto& entity : view)
			{
				auto [transformCompo, circleCompo] = view.get<TransformComponent, CircleRenderComponent>(entity);

				Renderer2D::DrawCircle(transformCompo.GetTransform(), circleCompo, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

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
	void Scene::OnComponentAdd<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdd<SpriteRenderComponent>(Entity entity, SpriteRenderComponent& component)
	{}

	template<>
	void Scene::OnComponentAdd<CircleRenderComponent>(Entity entity, CircleRenderComponent& component)
	{}

	template<>
	void Scene::OnComponentAdd<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{}

	template<>
	void Scene::OnComponentAdd<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{}

	template<>
	void Scene::OnComponentAdd<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{}
}
