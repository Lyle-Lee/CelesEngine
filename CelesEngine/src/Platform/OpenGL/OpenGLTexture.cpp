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

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		CE_CORE_ASSERT(internalFormat && dataFormat, "Incompatible data format!")

		glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID);
		glTextureStorage2D(m_BufferID, 1, internalFormat, width, height);

		glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(int width, int height, GLenum internalFormat, GLenum dataType, bool csStorage, int levels)
		: m_Width(width), m_Height(height)
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

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		//glBindTextures(slot, 1, &m_BufferID);
		glBindTextureUnit(slot, m_BufferID);
	}

}
