#ifndef ImGuiOpenGLContext_H
#define ImGuiOpenGLContext_H
/********************************
 * @Project: Zero
 * @Brief: ImGui������
 * @Copy Author: YangCong
 * @Date: 2019-02-18
 ********************************/
#include "imgui.h"
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace ImGui {
	class ImGuiOpenGLContext
	{
	public:
		typedef std::shared_ptr<ImGuiOpenGLContext> ptr;

		ImGuiOpenGLContext(GLFWwindow* window);

		~ImGuiOpenGLContext();

		//! ��ʼ��.
		void Init(GLFWwindow* window);

		//! �ر�.
		void Shutdown();

		//! ֡��Ⱦ��ʼ.
		void BeginFrame();

		//! ֡��Ⱦ����.
		void EndFrame();

		//! �����豸objects.
		void InvalidateDeviceObjects();

		//! �����豸objects.
		bool CreateDeviceObjects();

	private:
		bool CreateFontsTexture();

		void PushOpenGLState();
		void PopOpenGLState();

		/* ��Ⱦ���� */
		static void RenderDrawLists(ImDrawData* draw_data);

		static const char* GetClipboardText(void* user_data);

		static void SetClipboardText(void* user_data, const char* text);

		/* �����ص����� */
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

		/* �ַ�����ص� */
		static void CharCallback(GLFWwindow* window, unsigned int c);

		/* ��갴���ص����� */
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int modify);

		/* �����ֻص����� */
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		GLFWwindow* pWindow;

		double fTime;

		//! ����ֵ.
		float  fMouseWheel;

		//! �����������װ��.
		bool mousePressed[3];

		/* �������� */
		GLuint uFontTexture;

		/* ��ɫ�� */
		int nShaderHandle;
		int nVertHandle;
		int nFragHandle;

		/* ��ɫ�������ݰ󶨵� */
		int nAttribLocationTex;
		int nAttribLocationProjMtx;

		int nAttribLocationPosition;
		int nAttribLocationUV;
		int nAttribLocationColor;

		/* OpenGL ������ */
		unsigned int uVBOHandle;
		unsigned int uVAOHandle;
		unsigned int uElementsHandle;

		/* OpenGL ״̬ */
		GLint last_active_texture;
		GLint last_program;
		GLint last_texture;
		GLint last_array_buffer;
		GLint last_element_array_buffer;
		GLint last_vertex_array;
		GLint last_blend_src_rgb;
		GLint last_blend_dst_rgb;
		GLint last_blend_src_alpha;
		GLint last_blend_dst_alpha;
		GLint last_blend_equation_rgb;
		GLint last_blend_equation_alpha;
		GLint last_viewport[4];
		GLint last_scissor_box[4];

		GLboolean last_enable_blend;
		GLboolean last_enable_cull_face;
		GLboolean last_enable_depth_test;
		GLboolean last_enable_scissor_test;
	};

}
#endif