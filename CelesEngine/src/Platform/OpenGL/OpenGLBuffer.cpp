#include "PCH.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include <GL/glew.h>

namespace Celes {

	static const uint32_t s_MaxFrameBufferSize = 8192;

	// Vertex Buffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	// Index Buffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t size)
		: m_Count(size / sizeof(uint32_t))
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Frame Buffer
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferDescription& fbDesc): m_Width(fbDesc.Width), m_Height(fbDesc.Height)
	{
		glGenFramebuffers(1, &m_BufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

		/*glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_Width, m_Height);*/

		size_t n = fbDesc.AttachmentDesc.Attachments.size();
		m_Textures.reserve(n);
		m_Attachments.reserve(n);
		for (size_t i = 0; i < n; ++i)
		{
			AddAttachment(fbDesc.AttachmentDesc.Attachments[i]);
		}

		SetRenderBuffer();
		Unbind();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_BufferID);
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
		//glEnable(GL_DEPTH_TEST);
	}

	void OpenGLFrameBuffer::Unbind() const
	{
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::SetRenderBuffer() const
	{
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
		if (m_ColorAttachmentsCnt > 1)
		{
			CE_CORE_ASSERT(m_ColorAttachmentsCnt <= 4, "Exceeded color attachments max limit!")
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachmentsCnt, buffers);
		}
		else if (m_ColorAttachmentsCnt == 0)
		{
			// Depth-pass only
			glDrawBuffer(GL_NONE);
		}

		CE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")
	}

	void OpenGLFrameBuffer::AddAttachment(const FrameBufferTextureDescription& texDesc)
	{
		auto texture = Texture2D::Create(m_Width, m_Height, texDesc.TexFormat, texDesc.Samples);
		texture->Bind();

		m_Textures.push_back(texture);
		GLenum slot;
		switch (texDesc.TexFormat)
		{
		case TextureFormat::RGBA8:
			slot = GL_COLOR_ATTACHMENT0 + m_ColorAttachmentsCnt++;
			break;
		case TextureFormat::R32INT:
			slot = GL_COLOR_ATTACHMENT0 + m_ColorAttachmentsCnt++;
			break;
		case TextureFormat::DEPTH16:
			slot = GL_DEPTH_ATTACHMENT;
			break;
		case TextureFormat::DEPTH32:
			slot = GL_DEPTH_ATTACHMENT;
			break;
		case TextureFormat::DEPTH24STENCIL8:
			slot = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			CE_CORE_ASSERT(false, "Incompatible data format!")
			break;
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, slot, texture->IsMultiSampled() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, texture->GetBufferID(), 0);
		m_Attachments.push_back(slot);
	}

	uint32_t OpenGLFrameBuffer::GetAttachmentBufferID(size_t idx)
	{
		if (idx >= m_Textures.size())
			return 0u;

		return m_Textures[idx]->GetBufferID();
	}

	Ref<Texture2D>& OpenGLFrameBuffer::GetAttachmentTexture(size_t idx)
	{
		CE_CORE_ASSERT(idx < m_Textures.size(), "Access attachment texture violation!")
		return m_Textures[idx];
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			CE_CORE_WARN("Attempted to resize frame buffer to ({0}, {1})", width, height);
			return;
		}

		if (m_BufferID) glDeleteFramebuffers(1, &m_BufferID);

		m_Width = width;
		m_Height = height;

		glGenFramebuffers(1, &m_BufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

		/*glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);*/

		size_t n = m_Textures.size();
		if (n)
		{
			Bind();
			for (int i = 0; i < n; ++i)
			{
				m_Textures[i]->Resize(width, height);
				glFramebufferTexture2D(GL_FRAMEBUFFER, m_Attachments[i], m_Textures[i]->IsMultiSampled() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, m_Textures[i]->GetBufferID(), 0);
			}

			SetRenderBuffer();
			Unbind();
		}
	}

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIdx, int x, int y)
	{
		CE_CORE_ASSERT(attachmentIdx < m_Attachments.size(), "Access attachment texture violation!")

		glReadBuffer(m_Attachments[attachmentIdx]);
		int pixelData;
		glReadPixels(x, y, 1, 1, std::dynamic_pointer_cast<OpenGLTexture2D>(m_Textures[attachmentIdx])->GetDataFormat(), GL_INT, &pixelData);
		
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIdx, int value)
	{
		CE_CORE_ASSERT(attachmentIdx < m_Attachments.size(), "Access attachment texture violation!")

		glClearTexImage(GetAttachmentBufferID(attachmentIdx), 0, std::dynamic_pointer_cast<OpenGLTexture2D>(m_Textures[attachmentIdx])->GetDataFormat(), GL_INT, &value);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t slot)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_BufferID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_BufferID, offset, size, data);
	}

}
