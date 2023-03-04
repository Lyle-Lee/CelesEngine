#pragma once

#include "Celes/Renderer/Texture.h"
#include <GL/glew.h>

namespace Celes {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height, GLenum internalFormat = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE, bool csStorage = false, int levels = 0);
		virtual ~OpenGLTexture2D();

		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }
		inline uint32_t GetBufferID() const override { return m_BufferID; }

		void SetData(void* data, uint32_t size) override;

		void Bind(uint32_t slot = 0) const override;
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height, m_BufferID;
		GLenum m_InternalFormat, m_DataType, m_DataFormat;
	};

}
