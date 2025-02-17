#pragma once

#include <string>
#include <glm.hpp>

namespace Celes {

	class CE_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int val) = 0;
		virtual void SetIntArray(const std::string& name, int* vals, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float val) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& vec) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& vec) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc, bool fromFile = false);
		static Ref<Shader> Create(const std::string& computeSrc, bool fromFile = false);
	};

	class ShaderLibrary
	{
	public:
		CE_API void Add(const Ref<Shader>& shader);
		CE_API bool Exists(const std::string& name) const;

		CE_API Ref<Shader> Load(const std::string& vsPath, const std::string& fsPath);
		CE_API Ref<Shader> Load(const std::string& path);
		CE_API Ref<Shader> Get(const std::string& name);

		CE_API inline static uint32_t GetAnonymityCnt() { return s_AnonymityCnt++; }
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
		static uint32_t s_AnonymityCnt;
	};

}
