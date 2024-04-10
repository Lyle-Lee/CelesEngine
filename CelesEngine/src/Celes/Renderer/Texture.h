#pragma once

#include <filesystem>
#include <glm.hpp>
#include "Celes/Core/Core.h"

namespace Celes {

	class OpenGLFrameBuffer;

	enum class TextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		R32INT,

		// Depth/Stencil
		DEPTH16,
		DEPTH32,
		DEPTH24STENCIL8
	};

	class CE_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetBufferID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class CE_API Texture2D : public Texture
	{
		friend class OpenGLFrameBuffer;
	public:
		static Ref<Texture2D> Create(const std::filesystem::path& path);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8, uint32_t sampleCnt = 1);
		bool IsDepthBuffer() const;
		inline bool IsMultiSampled() const { return m_SampleCnt > 1; }
	protected:
		Texture2D() = default;
		Texture2D(TextureFormat format, uint32_t sampleCnt = 1): m_Format(format), m_SampleCnt(sampleCnt) {}
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		TextureFormat m_Format = TextureFormat::None;
		uint32_t m_SampleCnt = 1; // For super sampling.
	};

	class CE_API SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& minBound, const glm::vec2& maxBound);

		inline const Ref<Texture2D> GetTexture() const { return m_Texture; }
		inline const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static Ref<SubTexture2D> CreateFromCoord(const Ref<Texture2D>& texture, const glm::vec2& coord, const glm::vec2& spriteSize, const glm::vec2& cellUnitCnt = { 1.0f, 1.0f });
	private:
		Ref<Texture2D> m_Texture;
		glm::vec2 m_TexCoords[4];
	};

}
