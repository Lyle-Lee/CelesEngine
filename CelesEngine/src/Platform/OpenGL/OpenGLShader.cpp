#include "PCH.h"
#include "OpenGLShader.h"
#include "Celes/Core/Core.h"
#include "Celes/Utils/Timer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <fstream>
#include <filesystem>

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
			in.read(result.data(), result.size());
			in.close();
		}
		else
		{
			CE_CORE_ERROR("Could not open file '{0}'", path);
		}

		return result;
	}

	static const char* GetCacheDirectory()
	{
		// TODO: make sure the assets directory is valid
		return "assets/cache/shader/opengl";
	}

	// Create cache directory if needed.
	static void CreateCacheDirectory()
	{
		std::filesystem::path cacheDirectory = GetCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directories(cacheDirectory);
	}

	static const char* GetCachedOpenGLFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER: return ".cached_opengl.vert";
		case GL_FRAGMENT_SHADER: return ".cached_opengl.frag";
		case GL_COMPUTE_SHADER: return ".cached_opengl.comp";
		default:
			CE_CORE_ASSERT(false, "Unknown shader stage!")
			break;
		}

		return "";
	}

	static const char* GetCachedVulkanFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER: return ".cached_vulkan.vs";
		case GL_FRAGMENT_SHADER: return ".cached_vulkan.fs";
		case GL_COMPUTE_SHADER: return ".cached_vulkan.cs";
		default:
			CE_CORE_ASSERT(false, "Unknown shader stage!")
			break;
		}

		return "";
	}

	static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER: return shaderc_glsl_vertex_shader;
		case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
		case GL_COMPUTE_SHADER: return shaderc_glsl_compute_shader;
		default:
			CE_CORE_ASSERT(false, "Unknown shader stage!")
			break;
		}

		return (shaderc_shader_kind)0;
	}

	static const char* GLShaderStageToString(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
		case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
		case GL_COMPUTE_SHADER: return "GL_COMPUTE_SHADER";
		default:
			CE_CORE_ASSERT(false, "Unknown shader stage!")
			break;
		}

		return nullptr;
	}

	OpenGLShader::OpenGLShader(const std::string& vs, const std::string& fs, bool fromFile)
	{
		CreateCacheDirectory();

		std::unordered_map<GLenum, std::string> srcs;
		srcs[GL_VERTEX_SHADER] = fromFile ? ReadFileAsString(vs) : vs;
		srcs[GL_FRAGMENT_SHADER] = fromFile ? ReadFileAsString(fs) : fs;

		if (fromFile)
		{
			// Extract name from filepath
			auto start = vs.find_last_of("/\\");
			start = start == std::string::npos ? 0 : start + 1;
			auto end = vs.rfind(".vert");
			auto count = end == std::string::npos ? vs.size() - start : end - start;
			m_Name = vs.substr(start, count);
		}
		else
		{
			m_Name = "Shader" + ('0' + ShaderLibrary::GetAnonymityCnt());
		}

		{
			Timer timer("Shader creation", [&](float dTime) { CE_CORE_TRACE("Created shader '{0}' in {1} ms.", m_Name, dTime); });
			CompileVulkan(srcs);
			CompileOpenGL();
			CreateProgram();
		}
	}

	OpenGLShader::OpenGLShader(const std::string& cs, bool fromFile)
	{
		CreateCacheDirectory();

		std::unordered_map<GLenum, std::string> srcs;
		srcs[GL_COMPUTE_SHADER] = fromFile ? ReadFileAsString(cs) : cs;

		if (fromFile)
		{
			// Extract name from filepath
			auto start = cs.find_last_of("/\\");
			start = start == std::string::npos ? 0 : start + 1;
			auto end = cs.rfind(".comp");
			auto count = end == std::string::npos ? cs.size() - start : end - start;
			m_Name = cs.substr(start, count);
		}
		else
		{
			m_Name = "Shader" + ('0' + ShaderLibrary::GetAnonymityCnt());  
		}

		{
			Timer timer("Shader creation", [&](float dTime) { CE_CORE_TRACE("Created shader '{0}' in {1} ms.", m_Name, dTime); });
			CompileVulkan(srcs);
			CompileOpenGL();
			CreateProgram();
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

	void OpenGLShader::SetInt(const std::string& name, int val)
	{
		SetUniformInt(name, val);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* vals, uint32_t count)
	{
		SetUniformIntArray(name, vals, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float val)
	{
		SetUniformFloat(name, val);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& vec)
	{
		SetUniformFloat3(name, vec);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& vec)
	{
		SetUniformFloat4(name, vec);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix)
	{
		SetUniformMat4(name, matrix);
	}

	void OpenGLShader::SetUniformInt(const std::string& name, int val)
	{
		glUniform1i(GetUniformLocation(name), val);
	}

	void OpenGLShader::SetUniformIntArray(const std::string& name, int* vals, uint32_t count)
	{
		glUniform1iv(GetUniformLocation(name), count, vals);
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

	void OpenGLShader::CompileOpenGL()
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		//options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDir = GetCacheDirectory();
		m_OpenGLSPIRV.clear();
		m_OpenGLSrc.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path cachedPath = cacheDir / (m_Name + GetCachedOpenGLFileExtension(stage));
			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = m_OpenGLSPIRV[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSrc[stage] = glslCompiler.compile();
				auto& src = m_OpenGLSrc[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(src, GLShaderStageToShaderC(stage), m_Name.c_str());
				CE_CORE_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage())

				m_OpenGLSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = m_OpenGLSPIRV[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CompileVulkan(const std::unordered_map<GLenum, std::string>& shaderSrcs)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDir = GetCacheDirectory();

		m_VulkanSPIRV.clear();
		for (auto&& [stage, src] : shaderSrcs)
		{
			std::filesystem::path cachedPath = cacheDir / (m_Name + GetCachedOpenGLFileExtension(stage));
			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = m_VulkanSPIRV[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(src, GLShaderStageToShaderC(stage), m_Name.c_str(), options);
				CE_CORE_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage())

				m_VulkanSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = m_VulkanSPIRV[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : m_VulkanSPIRV) Reflect(stage, data);
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

			glDeleteProgram(program);
			for (auto id : shaderIDs) glDeleteShader(id);

			CE_CORE_ERROR("Failed to link shader '{0}':\n{1}", m_Name, infoLog.data());
		}

		m_Program = program;
		for (auto id : shaderIDs) glDetachShader(program, id);
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData) const
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		CE_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", GLShaderStageToString(stage), m_Name);
		CE_CORE_TRACE("--- {0} uniform buffer(s) ---", resources.uniform_buffers.size());
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCnt = bufferType.member_types.size();

			CE_CORE_TRACE("{0}", resource.name);
			CE_CORE_TRACE("    Size = {0}", bufferSize);
			CE_CORE_TRACE("    Binding = {0}", binding);
			CE_CORE_TRACE("    Member count = {0}", memberCnt);
		}

		CE_CORE_TRACE("--- {0} resources ---", resources.sampled_images.size());
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
