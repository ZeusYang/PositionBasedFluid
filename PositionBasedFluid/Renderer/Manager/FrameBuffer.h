#pragma once

#include <GL/glew.h>
#include <memory>

namespace Renderer
{
	class FrameBuffer
	{
	public:
		enum BufferType { DepthBuffer, ColorBuffer, StencilBuffer };

	private:
		unsigned int m_id;
		BufferType m_type;
		int m_width, m_height;
		unsigned int m_depthIndex;

	public:
		typedef std::shared_ptr<FrameBuffer> ptr;


		FrameBuffer(int width, int height, BufferType type);
		virtual ~FrameBuffer() = default;

		static GLenum getGLtype(BufferType type);
		static GLenum getGLAttach(BufferType type);

		void bind();
		static void unBind(int width, int height);

	private:
		void setupDepthFramebuffer();
		void clearFramebuffer();
	};

}
