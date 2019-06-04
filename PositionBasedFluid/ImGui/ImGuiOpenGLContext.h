#ifndef ImGuiOpenGLContext_H
#define ImGuiOpenGLContext_H
/********************************
 * @Project: Zero
 * @Brief: ImGui上下文
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

		//! 初始化.
		void Init(GLFWwindow* window);

		//! 关闭.
		void Shutdown();

		//! 帧渲染起始.
		void BeginFrame();

		//! 帧渲染结束.
		void EndFrame();

		//! 销毁设备objects.
		void InvalidateDeviceObjects();

		//! 创建设备objects.
		bool CreateDeviceObjects();

	private:
		bool CreateFontsTexture();

		void PushOpenGLState();
		void PopOpenGLState();

		/* 渲染函数 */
		static void RenderDrawLists(ImDrawData* draw_data);

		static const char* GetClipboardText(void* user_data);

		static void SetClipboardText(void* user_data, const char* text);

		/* 按键回调函数 */
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

		/* 字符输出回调 */
		static void CharCallback(GLFWwindow* window, unsigned int c);

		/* 鼠标按键回调函数 */
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int modify);

		/* 鼠标滚轮回调函数 */
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		GLFWwindow* pWindow;

		double fTime;

		//! 滚轮值.
		float  fMouseWheel;

		//! 鼠标三个键的装填.
		bool mousePressed[3];

		/* 字体纹理 */
		GLuint uFontTexture;

		/* 着色器 */
		int nShaderHandle;
		int nVertHandle;
		int nFragHandle;

		/* 着色程序数据绑定点 */
		int nAttribLocationTex;
		int nAttribLocationProjMtx;

		int nAttribLocationPosition;
		int nAttribLocationUV;
		int nAttribLocationColor;

		/* OpenGL 缓冲区 */
		unsigned int uVBOHandle;
		unsigned int uVAOHandle;
		unsigned int uElementsHandle;

		/* OpenGL 状态 */
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