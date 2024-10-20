#pragma once

#include "Celes/Renderer/Shader.h"
#include <glm.hpp>

// Temporary
typedef unsigned int GLenum;
typedef int GLint;

namespace Celes {

	class OpenGLShader : public Shader
	{
	public:
		CE_API OpenGLShader(const std::string& vs, const std::string& fs, bool fromFile = false);
		CE_API OpenGLShader(const std::string& cs, bool fromFile = false);
		CE_API virtual ~OpenGLShader();

		CE_API void Bind() const override;
		CE_API void Unbind() const override;

		CE_API void SetInt(const std::string& name, int val) override;
		CE_API void SetIntArray(const std::string& name, int* vals, uint32_t count) override;
		CE_API void SetFloat(const std::string& name, float val) override;
		CE_API void SetFloat3(const std::string& name, const glm::vec3& vec) override;
		CE_API void SetFloat4(const std::string& name, const glm::vec4& vec) override;
		CE_API void SetMat4(const std::string& name, const glm::mat4& matrix) override;

		CE_API inline const std::string& GetName() const override { return m_Name; }

		CE_API void SetUniformInt(const std::string& name, int val);
		CE_API void SetUniformIntArray(const std::string& name, int* vals, uint32_t count);

		CE_API void SetUniformFloat(const std::string& name, float val);
		CE_API void SetUniformFloat2(const std::string& name, const glm::vec2& vec);
		CE_API void SetUniformFloat3(const std::string& name, const glm::vec3& vec);
		CE_API void SetUniformFloat4(const std::string& name, const glm::vec4& vec);

		CE_API void SetUniformMat3(const std::string& name, const glm::mat3& matrix);
		CE_API void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		void CompileOpenGL();
		void CompileVulkan(const std::unordered_map<GLenum, std::string>& shaderSrcs);
		void CreateProgram();
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData) const;

		GLint GetUniformLocation(const std::string& name);

		uint32_t m_Program;
		std::string m_Name;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<GLenum, std::string> m_OpenGLSrc;

		mutable std::unordered_map<std::string, int> m_UniformLocationCache;
		mutable std::unordered_set<std::string> m_LogCache;
	};

}
