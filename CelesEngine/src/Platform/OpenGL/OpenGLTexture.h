#pragma once

#include "Celes/Renderer/Texture.h"
#include <GL/glew.h>

namespace Celes {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat internalFormat, uint32_t sampleCnt, GLenum dataType = GL_UNSIGNED_BYTE, uint32_t levels = 0);
		virtual ~OpenGLTexture2D();

		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }
		inline uint32_t GetBufferID() const override { return m_BufferID; }
		inline GLenum GetDataFormat() const { return m_DataFormat; }

		void SetData(void* data, uint32_t size) override;

		void Bind(uint32_t slot = 0) const override;

		void Resize(uint32_t width, uint32_t height) override;

		bool operator==(const Texture& other) const override { return m_BufferID == other.GetBufferID(); }
	private:
		void SetResource();

		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_BufferID = 0;
		GLenum m_InternalFormat, m_DataType, m_DataFormat;
		uint32_t m_MipLevel = 0;
	};

}
