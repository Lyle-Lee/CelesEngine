#pragma once

#include "Celes/Renderer/Buffer.h"

namespace Celes {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		void Bind() const override;
		void Unbind() const override;
		inline void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		inline const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetData(const void* data, uint32_t size) override;
	private:
		uint32_t m_BufferID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;
		inline uint32_t GetCount() const override { return m_Count; }
	private:
		uint32_t m_BufferID;
		uint32_t m_Count;
	};

	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(uint32_t width, uint32_t height);
		virtual ~OpenGLFrameBuffer();

		void Bind() const override;
		void UnBind() const override;
		void SetRenderBuffer() const override;

		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }

		void AddAttachment(const Ref<Texture>& texture) override;
	private:
		uint32_t m_BufferID, m_RenderBufferID;
		uint32_t m_Width, m_Height;
		std::vector<Ref<Texture>> m_Textures;
		std::vector<unsigned int> m_Attachments;
	};

}