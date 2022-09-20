#include "VertexArray.h"
#include "Utils.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_BufferID);
	glBindVertexArray(m_BufferID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_BufferID);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& vbLayout)
{
	this->bind();
	vb.bind();
	const auto& elements = vbLayout.getElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		glEnableVertexAttribArray(i);
		GLCall(glVertexAttribPointer(i, elements[i].count, elements[i].type, elements[i].normalized, vbLayout.getStride(), (const void*)offset))
		offset += elements[i].count * VertexBufferElement::getSizeOfType(elements[i].type);
	}
}

void VertexArray::bind() const
{
	glBindVertexArray(m_BufferID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}
