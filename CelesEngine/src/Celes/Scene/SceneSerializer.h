#pragma once

#include "Scene.h"

namespace Celes {

	class SceneSerializer
	{
	public:
		CE_API SceneSerializer(const Ref<Scene>& scene);

		CE_API void Serialize(const std::string& filepath);
		CE_API void SerializeRuntime(const std::string& filepath);

		CE_API bool Deserialize(const std::string& filepath);
		CE_API bool DeserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};

}
