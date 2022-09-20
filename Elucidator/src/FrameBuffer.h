#pragma once

#include "Texture.h"

class FrameBuffer
{
private:
	unsigned int m_BufferID;
	unsigned int m_RenderBufferID = 0;
	int m_Width, m_Height;
public:
	FrameBuffer(const int width, const int height);
	~FrameBuffer();

	void bindTexture(const Texture& texture, unsigned int slot = 0) const;
	void bindRenderBuffer() const;

	void bind() const;
	void unbind() const;
	void unbindRenderBuffer() const;

	void checkStatus() const;
};