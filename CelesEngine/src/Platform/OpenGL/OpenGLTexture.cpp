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

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureFormat internalFormat, uint32_t sampleCnt, GLenum dataType, uint32_t levels)
		:Texture2D(internalFormat, sampleCnt), m_Width(width), m_Height(height), m_DataType(dataType), m_DataFormat(GL_RGBA), m_MipLevel(levels)
	{
		switch (internalFormat)
		{
		case TextureFormat::None:
			CE_CORE_ERROR("Incompatible data format!");
			m_InternalFormat = GL_RGBA8;
			break;
		case TextureFormat::RGBA8:
			m_InternalFormat = GL_RGBA8;
			break;
		case TextureFormat::R32INT:
			m_InternalFormat = GL_R32I;
			m_DataFormat = GL_RED_INTEGER;
			break;
		case TextureFormat::DEPTH16:
			m_InternalFormat = GL_DEPTH_COMPONENT16;
			break;
		case TextureFormat::DEPTH32:
			m_InternalFormat = GL_DEPTH_COMPONENT32;
			break;
		case TextureFormat::DEPTH24STENCIL8:
			m_InternalFormat = GL_DEPTH24_STENCIL8;
			break;
		default:
			m_InternalFormat = GL_RGBA8;
			break;
		}

		SetResource();
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

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		if (m_BufferID) glDeleteTextures(1, &m_BufferID);

		m_Width = width;
		m_Height = height;

		SetResource();
	}

	void OpenGLTexture2D::SetResource()
	{
		GLenum target = IsMultiSampled() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glGenTextures(1, &m_BufferID);
		glBindTexture(target, m_BufferID);

		if (IsDepthBuffer())
		{
			glTexStorage2D(GL_TEXTURE_2D, m_MipLevel + 1, m_InternalFormat, m_Width, m_Height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // horizontal clamp
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // vertical clamp
		}
		else
		{
			if (IsMultiSampled())
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SampleCnt, m_InternalFormat, m_Width, m_Height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, m_MipLevel, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, m_DataType, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // horizontal clamp
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // vertical clamp
			}
		}
	}

}
