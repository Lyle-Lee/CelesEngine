#include "PCH.h"
#include "OpenGLTexture.h"
#include <stb_image.h>

namespace Celes {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path): m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		CE_CORE_ASSERT(data, "Failed to load image!")

		m_Width = width;
		m_Height = height;

		m_InternalFormat = 0;
		m_DataFormat = 0;
		m_DataType = GL_UNSIGNED_BYTE;
		if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}
		CE_CORE_ASSERT(m_InternalFormat && m_DataFormat, "Incompatible data format!")

		glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID);
		glTextureStorage2D(m_BufferID, 1, m_InternalFormat, width, height);

		glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height, m_DataFormat, m_DataType, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, GLenum internalFormat, GLenum dataType, bool csStorage, int levels)
		: m_Width(width), m_Height(height), m_InternalFormat(internalFormat), m_DataType(dataType), m_DataFormat(GL_RGBA)
	{
		glGenTextures(1, &m_BufferID);
		glBindTexture(GL_TEXTURE_2D, m_BufferID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // horizontal clamp
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // vertical clamp

		if (csStorage)
		{
			glTexStorage2D(GL_TEXTURE_2D, levels, internalFormat, m_Width, m_Height);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, GL_RGBA, dataType, nullptr);
			if (levels > 0)
				glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_BufferID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		CE_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Data must be entire texture!")

		//glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, m_DataType, data);
		glTextureSubImage2D(m_BufferID, 0, 0, 0, m_Width, m_Height, m_DataFormat, m_DataType, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		//glBindTextures(slot, 1, &m_BufferID);
		glBindTextureUnit(slot, m_BufferID);
	}

}
