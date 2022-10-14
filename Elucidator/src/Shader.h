#pragma once

#include <string>
#include <unordered_map>

#include <glm.hpp>

enum class ShaderType
{
	NONE = -1, VERTEX = 0, FRAGMENT = 1
};

class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_VertexProgramSource, m_FragmentProgramSource;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;

	unsigned int compileShader(unsigned int type, const std::string& source);
	unsigned int createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	unsigned int createShader(const std::string& computeShaderPath);

	int getUniformLocation(const std::string& name);
public:
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	Shader(const std::string& computeShaderPath);
	~Shader();

	void bind() const;
	void unbind() const;

	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform4fv(const std::string& name, int count, const float* valueVector);
	void setUniform3f(const std::string& name, float v0, float v1, float v2);
	void setUniform3fv(const std::string& name, int count, const float* valueVector);
	void setUniform1i(const std::string& name, int value);
	void setUniform1iv(const std::string& name, int count, const int* valueVector);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
};