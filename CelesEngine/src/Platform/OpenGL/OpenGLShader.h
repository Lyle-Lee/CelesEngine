#pragma once

#include "Celes/Renderer/Shader.h"
#include <glm.hpp>

// Temporary
typedef unsigned int GLenum;
typedef int GLint;

namespace Celes {

	class CE_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vs, const std::string& fs, bool fromFile = false);
		OpenGLShader(const std::string& cs, bool fromFile = false);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		void SetInt(const std::string& name, int val) override;
		void SetIntArray(const std::string& name, int* vals, uint32_t count) override;
		void SetFloat(const std::string& name, float val) override;
		void SetFloat3(const std::string& name, const glm::vec3& vec) override;
		void SetFloat4(const std::string& name, const glm::vec4& vec) override;
		void SetMat4(const std::string& name, const glm::mat4& matrix) override;

		inline const std::string& GetName() const override { return m_Name; }

		void SetUniformInt(const std::string& name, int val);
		void SetUniformIntArray(const std::string& name, int* vals, uint32_t count);

		void SetUniformFloat(const std::string& name, float val);
		void SetUniformFloat2(const std::string& name, const glm::vec2& vec);
		void SetUniformFloat3(const std::string& name, const glm::vec3& vec);
		void SetUniformFloat4(const std::string& name, const glm::vec4& vec);

		void SetUniformMat3(const std::string& name, const glm::mat3& matrix);
		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSrcs);
		GLint GetUniformLocation(const std::string& name);

		uint32_t m_Program;
		std::string m_Name;

		mutable std::unordered_map<std::string, int> m_UniformLocationCache;
		mutable std::unordered_set<std::string> m_LogCache;
	};

}
