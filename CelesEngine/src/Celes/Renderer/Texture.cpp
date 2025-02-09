#include "PCH.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Celes {

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLTexture2D>(path.string());
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFormat format, uint32_t sampleCnt)
	{
		switch (Renderer::GetAPI())
		{
			case GraphicsAPI::None:
				CE_CORE_ASSERT(false, "No Graphics API!")
				break;
			case GraphicsAPI::OpenGL:
				return CreateRef<OpenGLTexture2D>(width, height, format, sampleCnt);
			default:
				CE_CORE_ASSERT(false, "The target Graphics API is currently not supported!")
				break;
		}

		return nullptr;
	}

	bool Texture2D::IsDepthBuffer() const
	{
		switch (m_Format)
		{
		case TextureFormat::DEPTH16:
		case TextureFormat::DEPTH32:
		case TextureFormat::DEPTH24STENCIL8:
			return true;
		default:
			break;
		}

		return false;
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
