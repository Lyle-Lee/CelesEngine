#include <PCH.h>
#include "OpenGLVertexArray.h"
#include <GL/glew.h>

namespace Celes {

	static GLenum ShaderDataTypeToGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Celes::ShaderDataType::Float:
			return GL_FLOAT;
		case Celes::ShaderDataType::Float2:
			return GL_FLOAT;
		case Celes::ShaderDataType::Float3:
			return GL_FLOAT;
		case Celes::ShaderDataType::Float4:
			return GL_FLOAT;
		case Celes::ShaderDataType::Mat3:
			return GL_FLOAT;
		case Celes::ShaderDataType::Mat4:
			return GL_FLOAT;
		case Celes::ShaderDataType::Int:
			return GL_INT;
		case Celes::ShaderDataType::Int2:
			return GL_INT;
		case Celes::ShaderDataType::Int3:
			return GL_INT;
		case Celes::ShaderDataType::Int4:
			return GL_INT;
		case Celes::ShaderDataType::Bool:
			return GL_BOOL;
		default:
			break;
		}

		CE_CORE_ASSERT(false, "Unknown data type!")
			return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_BufferID);
		glBindVertexArray(m_BufferID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_BufferID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_BufferID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vb)
	{
		CE_CORE_ASSERT(vb->GetLayout().GetElements().size(), "No layout for vertex buffer!")

		glBindVertexArray(m_BufferID);
		vb->Bind();

		uint32_t index = 0;
		for (const auto& element : vb->GetLayout())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.GetComponentCount(),
				ShaderDataTypeToGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				vb->GetLayout().GetStride(),
				(const void*)element.Offset);
			++index;
		}

		m_VertexBuffers.push_back(vb);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& ib)
	{
		glBindVertexArray(m_BufferID);
		ib->Bind();

		m_IndexBuffer = ib;
	}

}
