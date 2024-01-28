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
		OpenGLFrameBuffer(const FrameBufferDesc& fbDesc);
		virtual ~OpenGLFrameBuffer();

		void Bind() const override;
		void Unbind() const override;
		void SetRenderBuffer() const override;

		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }

		void AddAttachment(const FrameBufferTextureDesc& texDesc) override;
		uint32_t GetAttachmentBufferID(size_t idx) override;
		Ref<Texture2D>& GetAttachmentTexture(size_t idx) override;

		void Resize(uint32_t width, uint32_t height) override;
	private:
		uint32_t m_BufferID = 0;
		uint32_t m_Width, m_Height;
		std::vector<Ref<Texture2D>> m_Textures;
		std::vector<uint32_t> m_Attachments;
		uint32_t m_ColorAttachmentsCnt = 0;
	};

}
