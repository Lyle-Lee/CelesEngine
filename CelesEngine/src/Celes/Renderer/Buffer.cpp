#include "PCH.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Celes {
	
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLVertexBuffer>(size);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLVertexBuffer>(vertices, size);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLIndexBuffer>(indices, size);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferDesc& fbDesc)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLFrameBuffer>(fbDesc);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t slot)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(size, slot);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

}
