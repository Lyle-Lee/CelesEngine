#pragma once

#include <string>

namespace Celes {

	class CE_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc, bool fromFile = false);
		static Ref<Shader> Create(const std::string& computeSrc, bool fromFile = false);
	};

	class CE_API ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		bool Exists(const std::string& name) const;

		Ref<Shader> Load(const std::string& vsPath, const std::string& fsPath);
		Ref<Shader> Load(const std::string& path);
		Ref<Shader> Get(const std::string& name);

		inline static uint32_t GetAnonymityCnt() { return s_AnonymityCnt++; }
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
		static uint32_t s_AnonymityCnt;
	};

}
