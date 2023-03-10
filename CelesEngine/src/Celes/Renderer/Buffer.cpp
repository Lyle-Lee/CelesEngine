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

	Ref<FrameBuffer> FrameBuffer::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLFrameBuffer>(width, height);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

}
