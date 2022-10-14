#include <iostream>
#include "Shader.h"
#include "Utils.h"

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    GLCall(glCompileShader(id))

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : type == GL_FRAGMENT_SHADER ? "fragment" : "compute") << " shader:" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::createShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    std::string vertexProgramSource = ReadFileAsString(vertexShaderPath);
    std::string fragmentProgramSource = ReadFileAsString(fragmentShaderPath);

    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexProgramSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentProgramSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int Shader::createShader(const std::string& computeShaderPath)
{
    std::string computeProgramSource = ReadFileAsString(computeShaderPath);

    unsigned int program = glCreateProgram();
    unsigned int cs = compileShader(GL_COMPUTE_SHADER, computeProgramSource);

    glAttachShader(program, cs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(cs);

    return program;
}

int Shader::getUniformLocation(const std::string& name)
{
    int location;
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        location = m_UniformLocationCache[name];
    }
    else
    {
        location = glGetUniformLocation(m_RendererID, name.c_str());
        m_UniformLocationCache[name] = location;
    }

    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' does not exist!" << std::endl;

    return location;
}

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    m_RendererID = createShader(vertexShaderPath, fragmentShaderPath);
    glUseProgram(m_RendererID);
}

Shader::Shader(const std::string& computeShaderPath)
{
    m_RendererID = createShader(computeShaderPath);
    glUseProgram(m_RendererID);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform4fv(const std::string& name, int count, const float* valueVector)
{
    glUniform4fv(getUniformLocation(name), count, valueVector);
}

void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Shader::setUniform3fv(const std::string& name, int count, const float* valueVector)
{
    glUniform3fv(getUniformLocation(name), count, valueVector);
}

void Shader::setUniform1i(const std::string& name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1iv(const std::string& name, int count, const int* valueVector)
{
    glUniform1iv(getUniformLocation(name), count, valueVector);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}
