#include <PCH.h>
#include "VertexArray.h"
#include "Renderer.h"
#include <Platform/OpenGL/OpenGLVertexArray.h>

namespace Celes {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return new OpenGLVertexArray();
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

}