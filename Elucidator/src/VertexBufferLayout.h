#pragma once

#include <vector>
#include "GL/glew.h"
#include "Utils.h"

struct VertexBufferElement
{
	unsigned int type;
	int count;
	bool normalized;

	static unsigned int getSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		default:
			break;
		}
		ASSERT(false)
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout():
		m_Stride(0) {}

	template<typename T>
	void push(int count)
	{
		//static_assert(false);
	}

	template<>
	void push<float>(int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, false });
		m_Stride += sizeof(GLfloat) * count;
	}

	template<>
	void push<unsigned int>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, false });
		m_Stride += sizeof(GLuint) * count;
	}

	template<>
	void push<unsigned char>(int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, true });
		m_Stride += sizeof(GLbyte) * count;
	}

	inline const std::vector<VertexBufferElement>& getElements() const { return m_Elements; }
	inline unsigned int getStride() const { return m_Stride; }
};