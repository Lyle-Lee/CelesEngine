#include "PCH.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Celes {

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLTexture2D>(path);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLTexture2D>(width, height);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& minBound, const glm::vec2& maxBound)
		: m_Texture(texture)
	{
		m_TexCoords[0] = { minBound.x, minBound.y };
		m_TexCoords[1] = { maxBound.x, minBound.y };
		m_TexCoords[2] = { maxBound.x, maxBound.y };
		m_TexCoords[3] = { minBound.x, maxBound.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoord(const Ref<Texture2D>& texture, const glm::vec2& coord, const glm::vec2& spriteSize, const glm::vec2& cellUnitCnt)
	{
		glm::vec2 minBound = { coord.x * spriteSize.x / texture->GetWidth(), coord.y * spriteSize.y / texture->GetHeight() };
		glm::vec2 maxBound = { (coord.x + cellUnitCnt.x) * spriteSize.x / texture->GetWidth(), (coord.y + cellUnitCnt.y) * spriteSize.y / texture->GetHeight() };

		return CreateRef<SubTexture2D>(texture, minBound, maxBound);
	}

}
