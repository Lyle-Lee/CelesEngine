#include <PCH.h>
#include "OpenGLShader.h"
#include <Celes/Core.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtc/type_ptr.hpp>
#include <fstream>

namespace Celes {

	static std::string ReadFileAsString(const std::string& path)
	{
		std::string result;
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			CE_CORE_ERROR("Could not open file '{0}'", path);
		}

		return result;
	}

	OpenGLShader::OpenGLShader(const std::string& vs, const std::string& fs, bool fromFile)
	{
		std::unordered_map<GLenum, std::string> srcs;
		srcs[GL_VERTEX_SHADER] = fromFile ? ReadFileAsString(vs) : vs;
		srcs[GL_FRAGMENT_SHADER] = fromFile ? ReadFileAsString(fs) : fs;
		
		Compile(srcs);

		if (fromFile)
		{
			auto start = vs.find_last_of("/\\");
			start = start == std::string::npos ? 0 : start + 1;
			auto end = vs.rfind("VertexShader");
			auto count = end == std::string::npos ? vs.size() - start : end - start;
			m_Name = vs.substr(start, count);
		}
		else
		{
			m_Name = "Shader" + ('0' + ShaderLibrary::GetAnonymityCnt());
		}
	}

	OpenGLShader::OpenGLShader(const std::string& cs, bool fromFile)
	{
		std::unordered_map<GLenum, std::string> srcs;
		srcs[GL_COMPUTE_SHADER] = fromFile ? ReadFileAsString(cs) : cs;

		Compile(srcs);

		if (fromFile)
		{
			auto start = cs.find_last_of("/\\");
			start = start == std::string::npos ? 0 : start + 1;
			auto end = cs.rfind("ComputeShader");
			auto count = end == std::string::npos ? cs.size() - start : end - start;
			m_Name = cs.substr(start, count);
		}
		else
		{
			m_Name = "Shader" + ('0' + ShaderLibrary::GetAnonymityCnt());  
		}
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_Program);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_Program);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformInt(const std::string& name, int val)
	{
		glUniform1i(GetUniformLocation(name), val);
	}

	void OpenGLShader::SetUniformFloat(const std::string& name, float val)
	{
		glUniform1f(GetUniformLocation(name), val);
	}

	void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& vec)
	{
		glUniform2f(GetUniformLocation(name), vec.x, vec.y);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& vec)
	{
		glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& vec)
	{
		glUniform4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
	}

	void OpenGLShader::SetUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSrcs)
	{
		CE_CORE_ASSERT(shaderSrcs.size() <= 2, "Incompatible shader group!")

		GLuint program = glCreateProgram();
		std::array<GLenum, 2> shaderTypes;
		int index = 0;
		for (auto& [type, src] : shaderSrcs)
		{
			GLuint shader = glCreateShader(type);

			const GLchar* source = src.c_str();
			glShaderSource(shader, 1, &source, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				CE_CORE_ERROR("{0}", infoLog.data());
				CE_CORE_ASSERT(false, "Failed to compile {0} shader!", type == GL_VERTEX_SHADER ? "vertex" : type == GL_FRAGMENT_SHADER ? "fragment" : "compute")
			}

			glAttachShader(program, shader);
			shaderTypes[index++] = type;
		}

		m_Program = program;

		glLinkProgram(m_Program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_Program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_Program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_Program);
			for (GLenum& type : shaderTypes) glDeleteShader(type);

			CE_CORE_ERROR("{0}", infoLog.data());
			CE_CORE_ASSERT(false, "Failed to link shaders!")
		}

		for (GLenum& type : shaderTypes) glDetachShader(m_Program, type);
	}

	GLint OpenGLShader::GetUniformLocation(const std::string& name)
	{
		GLint location;
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		{
			location = m_UniformLocationCache[name];
		}
		else
		{
			location = glGetUniformLocation(m_Program, name.c_str());
			m_UniformLocationCache[name] = location;
		}

		if (location == -1 && m_LogCache.find(name) == m_LogCache.end())
		{
			CE_CORE_WARN("Warning: uniform '{0}' does not exist!", name);
			m_LogCache.insert(name);
		}

		return location;
	}

}
