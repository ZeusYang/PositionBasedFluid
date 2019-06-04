#include "FrameBuffer.h"

#include "TextureMgr.h"
#include <iostream>

namespace Renderer
{
	FrameBuffer::FrameBuffer(int width, int height, BufferType type)
		:m_width(width), m_height(height), m_type(type)
	{
		setupDepthFramebuffer();
	}

	GLenum FrameBuffer::getGLtype(BufferType type)
	{
		switch (type) 
		{
		case DepthBuffer:
			return GL_DEPTH_COMPONENT;
		case ColorBuffer:
			return GL_RGB;
		case StencilBuffer:
			return GL_RGB;
		}
		return GL_RGB;
	}

	GLenum FrameBuffer::getGLAttach(BufferType type)
	{
		switch (type) 
		{
		case DepthBuffer:
			return GL_DEPTH_ATTACHMENT;
		case ColorBuffer:
			return GL_COLOR_ATTACHMENT0;
		case StencilBuffer:
			return GL_STENCIL_ATTACHMENT;
		}
		return GL_COLOR_ATTACHMENT0;
	}

	void FrameBuffer::bind()
	{
		glViewport(0, 0, m_width, m_height);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::unBind(int width, int height)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

	void FrameBuffer::setupDepthFramebuffer()
	{
		TextureMgr::ptr textureMgr = TextureMgr::getSingleton();
		m_depthIndex = textureMgr->loadTextureDepth("shadowDepth", m_width, m_height);
		glGenFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, textureMgr->getTexture(m_depthIndex)->getTextureId(), 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::clearFramebuffer()
	{
		glDeleteFramebuffers(1, &m_id);
	}

}