#include "PCH.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "EntityComponent.h"
#include <yaml-cpp/include/yaml-cpp/yaml.h>
#include <fstream>

namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			
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

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene): m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "12837192831273";

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

				Entity targetEntity = m_Scene->CreateEntity(name);

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
