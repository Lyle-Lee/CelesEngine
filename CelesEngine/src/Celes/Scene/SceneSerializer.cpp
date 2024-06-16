#include "PCH.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "EntityComponent.h"
#include <yaml-cpp/include/yaml-cpp/yaml.h>
#include <fstream>

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();

			return true;
		}
	};

}

namespace Celes {

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	static std::string Rigidbody2DTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Celes::Rigidbody2DComponent::BodyType::Static:
			return "Static";
		case Celes::Rigidbody2DComponent::BodyType::Dynamic:
			return "Dynamic";
		case Celes::Rigidbody2DComponent::BodyType::Kinematic:
			return "Kinematic";
		default:
			CE_CORE_ASSERT(false, "Unknown body type!")
			break;
		}

		return {};
	}

	static Rigidbody2DComponent::BodyType Rigidbody2DTypeFromString(const std::string& bodyTypeStr)
	{
		if (bodyTypeStr == "Static")
			return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeStr == "Dynamic")
			return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeStr == "Kinematic")
			return Rigidbody2DComponent::BodyType::Kinematic;

		CE_CORE_ASSERT(false, "Unknown body type!")
		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene): m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		CE_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Invalid entity!")
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto compo = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << compo.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << compo.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << compo.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& compo = entity.GetComponent<CameraComponent>();
			auto& camera = compo.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthoSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthoNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthoFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << compo.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << compo.FixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRenderComponent>())
		{
			out << YAML::Key << "SpriteRenderComponent";
			out << YAML::BeginMap;

			auto& compo = entity.GetComponent<SpriteRenderComponent>();
			out << YAML::Key << "Color" << YAML::Value << compo.Color;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRenderComponent>())
		{
			out << YAML::Key << "CircleRenderComponent";
			out << YAML::BeginMap;

			auto& compo = entity.GetComponent<CircleRenderComponent>();
			out << YAML::Key << "Color" << YAML::Value << compo.Color;
			out << YAML::Key << "Thickness" << YAML::Value << compo.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << compo.Fade;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap;

			auto& compo = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << Rigidbody2DTypeToString(compo.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << compo.FixedRotation;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& compo = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << compo.Offset;
			out << YAML::Key << "Size" << YAML::Value << compo.Size;
			out << YAML::Key << "Density" << YAML::Value << compo.Density;
			out << YAML::Key << "Friction" << YAML::Value << compo.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << compo.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << compo.RestitutionThreshold;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& compo = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << compo.Offset;
			out << YAML::Key << "Radius" << YAML::Value << compo.Radius;
			out << YAML::Key << "Density" << YAML::Value << compo.Density;
			out << YAML::Key << "Friction" << YAML::Value << compo.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << compo.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << compo.RestitutionThreshold;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity) return;

			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		CE_CORE_ASSERT(false, "Currently not implemented!")
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream ss;
		ss << stream.rdbuf();

		YAML::Node data = YAML::Load(ss.str());
		if (!data["Scene"]) return false;

		std::string sceneName = data["Scene"].as<std::string>();
		CE_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagCompo = entity["TagComponent"];
				if (tagCompo) name = tagCompo["Tag"].as<std::string>();

				CE_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity targetEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformCompo = entity["TransformComponent"];
				if (transformCompo)
				{
					// Entities always have transforms
					auto& compo = targetEntity.GetComponent<TransformComponent>();
					compo.Translation = transformCompo["Translation"].as<glm::vec3>();
					compo.Rotation = transformCompo["Rotation"].as<glm::vec3>();
					compo.Scale = transformCompo["Scale"].as<glm::vec3>();
				}

				auto cameraCompo = entity["CameraComponent"];
				if (cameraCompo)
				{
					auto& compo = targetEntity.AddComponent<CameraComponent>();

					auto cameraInfo = cameraCompo["Camera"];
					compo.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraInfo["ProjectionType"].as<int>());

					compo.Camera.SetPerspectiveFOV(cameraInfo["PerspectiveFOV"].as<float>());
					compo.Camera.SetPerspectiveNearClip(cameraInfo["PerspectiveNear"].as<float>());
					compo.Camera.SetPerspectiveFarClip(cameraInfo["PerspectiveFar"].as<float>());

					compo.Camera.SetOrthoSize(cameraInfo["OrthographicSize"].as<float>());
					compo.Camera.SetOrthoNearClip(cameraInfo["OrthographicNear"].as<float>());
					compo.Camera.SetOrthoFarClip(cameraInfo["OrthographicFar"].as<float>());

					compo.Primary = cameraCompo["Primary"].as<bool>();
					compo.FixedAspectRatio = cameraCompo["FixedAspectRatio"].as<bool>();
				}

				auto spriteRenderCompo = entity["SpriteRenderComponent"];
				if (spriteRenderCompo)
				{
					auto& compo = targetEntity.AddComponent<SpriteRenderComponent>();
					compo.Color = spriteRenderCompo["Color"].as<glm::vec4>();
				}

				auto circleRenderCompo = entity["CircleRenderComponent"];
				if (circleRenderCompo)
				{
					auto& compo = targetEntity.AddComponent<CircleRenderComponent>();
					compo.Color = circleRenderCompo["Color"].as<glm::vec4>();
					compo.Thickness = circleRenderCompo["Thickness"].as<float>();
					compo.Fade = circleRenderCompo["Fade"].as<float>();
				}

				auto rb2dCompo = entity["Rigidbody2DComponent"];
				if (rb2dCompo)
				{
					auto& compo = targetEntity.AddComponent<Rigidbody2DComponent>();
					compo.Type = Rigidbody2DTypeFromString(rb2dCompo["BodyType"].as<std::string>());
					compo.FixedRotation = rb2dCompo["FixedRotation"].as<bool>();
				}

				auto bc2dCompo = entity["BoxCollider2DComponent"];
				if (bc2dCompo)
				{
					auto& compo = targetEntity.AddComponent<BoxCollider2DComponent>();
					compo.Offset = bc2dCompo["Offset"].as<glm::vec2>();
					compo.Size = bc2dCompo["Size"].as<glm::vec2>();
					compo.Density = bc2dCompo["Density"].as<float>();
					compo.Friction = bc2dCompo["Friction"].as<float>();
					compo.Restitution = bc2dCompo["Restitution"].as<float>();
					compo.RestitutionThreshold = bc2dCompo["RestitutionThreshold"].as<float>();
				}

				auto cc2dCompo = entity["CircleCollider2DComponent"];
				if (cc2dCompo)
				{
					auto& compo = targetEntity.AddComponent<CircleCollider2DComponent>();
					compo.Offset = cc2dCompo["Offset"].as<glm::vec2>();
					compo.Radius = cc2dCompo["Radius"].as<float>();
					compo.Density = cc2dCompo["Density"].as<float>();
					compo.Friction = cc2dCompo["Friction"].as<float>();
					compo.Restitution = cc2dCompo["Restitution"].as<float>();
					compo.RestitutionThreshold = cc2dCompo["RestitutionThreshold"].as<float>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		CE_CORE_ASSERT(false, "Currently not implemented!")
		return false;
	}

}
