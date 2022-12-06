#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak(); // only supported by msvc
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLCheckError(#x, __FILE__, __LINE__))

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define RESOLUTION 2048

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "):" << std::endl;
        std::cout << "Last call: " << function << " trace back in " << file << " at line: " << line << std::endl;
        return false;
    }
    return true;
}

static std::string ReadFileAsString(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		result.resize((size_t)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else
	{
		std::cout << "Could not open file '" << filepath << "'" << std::endl;
	}

	return result;
}

class Timer
{
public:
	Timer(const char* name)
		: m_Name(name), m_Stopped(false)
	{
		m_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	void stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimePoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimePoint).time_since_epoch().count();

		std::cout << m_Name << ": " << (end - start) << "ms\n";

		m_Stopped = true;
	}

	~Timer()
	{
		if (!m_Stopped)
			stop();
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
	bool m_Stopped;
};