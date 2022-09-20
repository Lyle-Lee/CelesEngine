#pragma once

#include "Utils.h"

class Texture
{
private:
	unsigned int m_BufferID = 0;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer = nullptr;
	int m_Width, m_Height;
	int m_BPP = 32; // bits per pixel
public:
	Texture(const int width, const int height, GLint internalFormat = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
	Texture(const std::string& path);
	~Texture();

	void bind(unsigned int slot = 0) const;
	void unbind() const;
	void bindUnit(unsigned int index) const;

	inline int getWidth() const { return m_Width; }
	inline int getHeight() const { return m_Height; }
	inline unsigned int getBufferID() const { return m_BufferID; }
};