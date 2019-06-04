#include "ImGuiOpenGLContext.h"

namespace ImGui
{
	ImGuiOpenGLContext* imguiOpenGLContext;

	ImGuiOpenGLContext::ImGuiOpenGLContext(GLFWwindow* window)
	{
		imguiOpenGLContext = this;
		this->Init(window);
		uFontTexture = 0;
	}

	ImGuiOpenGLContext::~ImGuiOpenGLContext()
	{
		this->Shutdown();
	}

	void ImGuiOpenGLContext::Init(GLFWwindow* window)
	{
		pWindow = window;
		ImGuiIO& io = ImGui::GetIO();

		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		/* 设置渲染回调函数 */
		io.RenderDrawListsFn = ImGuiOpenGLContext::RenderDrawLists;

		/* 设置剪切板回调函数 */
		io.SetClipboardTextFn = ImGuiOpenGLContext::SetClipboardText;
		io.GetClipboardTextFn = ImGuiOpenGLContext::GetClipboardText;
		io.ClipboardUserData = pWindow;

		/* 设置消息回调函数 */
		glfwSetKeyCallback(pWindow, ImGuiOpenGLContext::KeyCallback);
		glfwSetCharCallback(pWindow, ImGuiOpenGLContext::CharCallback);
		glfwSetScrollCallback(pWindow, ImGuiOpenGLContext::ScrollCallback);
		glfwSetMouseButtonCallback(pWindow, ImGuiOpenGLContext::MouseButtonCallback);
	}

	void ImGuiOpenGLContext::Shutdown()
	{
		this->InvalidateDeviceObjects();
		ImGui::Shutdown();
	}

	void ImGuiOpenGLContext::BeginFrame()
	{
		if (!uFontTexture) {
			this->CreateDeviceObjects();
		}

		ImGuiIO& io = ImGui::GetIO();

		/* 显示区域大小（不一定和窗口大小相等） */
		static int w, h, display_w, display_h;
		glfwGetWindowSize(pWindow, &w, &h);
		glfwGetFramebufferSize(pWindow, &display_w, &display_h);

		io.DisplaySize = ImVec2(float(w), float(h));
		io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

		/* 帧间隔时间 */
		double current_time = glfwGetTime();
		io.DeltaTime = fTime >= 0 ? (float)(current_time - fTime) : float(1.0f / 60.0f);
		fTime = current_time;

		/* 光标坐标 */
		if (glfwGetWindowAttrib(pWindow, GLFW_FOCUSED)) {
			static double mousex, mousey;
			glfwGetCursorPos(pWindow, &mousex, &mousey);

			io.MousePos = ImVec2((float)mousex, (float)mousey);
		}
		else {
			io.MousePos = ImVec2(-1, -1);
		}

		/* 鼠标按键 */
		for (int i = 0; i < 3; i++) {
			io.MouseDown[i] = mousePressed[i] || glfwGetMouseButton(pWindow, i) != 0;
			mousePressed[i] = false;
		}

		io.MouseWheel = fMouseWheel;
		fMouseWheel = 0.0f;

		glfwSetInputMode(pWindow, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

		ImGui::NewFrame();
	}

	void ImGuiOpenGLContext::EndFrame()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ImGui::Render();
	}

	bool ImGuiOpenGLContext::CreateDeviceObjects()
	{
		GLint last_texture, last_array_buffer, last_vertex_array;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

		const GLchar *vertex_shader =
			"#version 330\n"
			"uniform mat4 ProjMtx;\n"
			"in vec2 Position;\n"
			"in vec2 UV;\n"
			"in vec4 Color;\n"
			"out vec2 Frag_UV;\n"
			"out vec4 Frag_Color;\n"
			"void main()\n"
			"{\n"
			"	Frag_UV = UV;\n"
			"	Frag_Color = Color;\n"
			"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
			"}\n";

		const GLchar* fragment_shader =
			"#version 330\n"
			"uniform sampler2D Texture;\n"
			"in vec2 Frag_UV;\n"
			"in vec4 Frag_Color;\n"
			"out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
			"}\n";

		/* 创建着色器 */
		nShaderHandle = glCreateProgram();
		nVertHandle = glCreateShader(GL_VERTEX_SHADER);
		nFragHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(nVertHandle, 1, &vertex_shader, 0);
		glShaderSource(nFragHandle, 1, &fragment_shader, 0);
		glCompileShader(nVertHandle);
		glCompileShader(nFragHandle);
		glAttachShader(nShaderHandle, nVertHandle);
		glAttachShader(nShaderHandle, nFragHandle);
		glLinkProgram(nShaderHandle);


		nAttribLocationTex = glGetUniformLocation(nShaderHandle, "Texture");
		nAttribLocationProjMtx = glGetUniformLocation(nShaderHandle, "ProjMtx");
		nAttribLocationPosition = glGetAttribLocation(nShaderHandle, "Position");
		nAttribLocationUV = glGetAttribLocation(nShaderHandle, "UV");
		nAttribLocationColor = glGetAttribLocation(nShaderHandle, "Color");

		/* 创建缓冲区 */
		glGenBuffers(1, &uVBOHandle);
		glGenBuffers(1, &uElementsHandle);

		glGenVertexArrays(1, &uVAOHandle);
		glBindVertexArray(uVAOHandle);
		glBindBuffer(GL_ARRAY_BUFFER, uVBOHandle);
		glEnableVertexAttribArray(nAttribLocationPosition);
		glEnableVertexAttribArray(nAttribLocationUV);
		glEnableVertexAttribArray(nAttribLocationColor);

		/* 设置顶点属性指针 */
		glVertexAttribPointer(nAttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos));
		glVertexAttribPointer(nAttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv));
		glVertexAttribPointer(nAttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col));

		this->CreateFontsTexture();

		glBindTexture(GL_TEXTURE_2D, last_texture);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindVertexArray(last_vertex_array);

		return true;
	}

	void ImGuiOpenGLContext::InvalidateDeviceObjects()
	{
		if (uVAOHandle) glDeleteVertexArrays(1, &uVAOHandle);
		if (uVBOHandle) glDeleteBuffers(1, &uVBOHandle);
		if (uElementsHandle) glDeleteBuffers(1, &uElementsHandle);
		uVAOHandle = uVBOHandle = uElementsHandle = 0;

		if (nShaderHandle && nVertHandle) {
			glDetachShader(nShaderHandle, nVertHandle);
		}
		if (nVertHandle) {
			glDeleteShader(nVertHandle);
		}
		nVertHandle = 0;

		if (nShaderHandle && nFragHandle) {
			glDetachShader(nShaderHandle, nFragHandle);
		}
		if (nFragHandle) {
			glDeleteShader(nFragHandle);
		}
		nFragHandle = 0;

		if (nShaderHandle) {
			glDeleteProgram(nShaderHandle);
		}
		nShaderHandle = 0;

		if (uFontTexture) {
			glDeleteTextures(1, &uFontTexture);
			ImGui::GetIO().Fonts->TexID = 0;
			uFontTexture = 0;
		}
	}

	bool ImGuiOpenGLContext::CreateFontsTexture()
	{
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &uFontTexture);
		glBindTexture(GL_TEXTURE_2D, uFontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		io.Fonts->TexID = (void *)(intptr_t)uFontTexture;

		glBindTexture(GL_TEXTURE_2D, last_texture);

		return true;
	}

	void ImGuiOpenGLContext::PushOpenGLState()
	{
		glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
		glActiveTexture(GL_TEXTURE0);
		glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

		glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
		glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
		glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);

		glGetIntegerv(GL_VIEWPORT, last_viewport);
		glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);

		last_enable_blend = glIsEnabled(GL_BLEND);
		last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
		last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
		last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
	}

	void ImGuiOpenGLContext::PopOpenGLState()
	{
		glUseProgram(last_program);
		glBindTexture(GL_TEXTURE_2D, last_texture);
		glActiveTexture(last_active_texture);

		glBindVertexArray(last_vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);

		glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
		glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
		if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);

		glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
	}

	void ImGuiOpenGLContext::RenderDrawLists(ImDrawData* draw_data)
	{
		ImGuiIO& io = ImGui::GetIO();

		int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);

		if (fb_width == 0 || fb_height == 0) return;
		draw_data->ScaleClipRects(io.DisplayFramebufferScale);

		/* 存储 OpenGL 原来的状态 */
		imguiOpenGLContext->PushOpenGLState();

		/* 设置 OpenGL 状态：开启裁剪测试、开启混合、禁止面剔除、禁止深度测试 */
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		/* 设置视图大小，正交矩阵 */
		glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
		const float ortho_projection[4][4] =
		{
			{ 2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
			{ 0.0f, 0.0f, -1.0f, 0.0f },
			{ -1.0f, 1.0f, 0.0f, 1.0f },
		};

		/* 设置着色程序 */
		glUseProgram(imguiOpenGLContext->nShaderHandle);
		glUniform1i(imguiOpenGLContext->nAttribLocationTex, 0);
		glUniformMatrix4fv(imguiOpenGLContext->nAttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
		glBindVertexArray(imguiOpenGLContext->uVAOHandle);

		/* 开始渲染 */
		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx* idx_buffer_offset = 0;

			/* 设置顶点数据到 VBO */
			glBindBuffer(GL_ARRAY_BUFFER, imguiOpenGLContext->uVBOHandle);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
				(const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

			/* 设置索引数据到 Ele */
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imguiOpenGLContext->uElementsHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
				(const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback) {
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else {
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor(
						(int)pcmd->ClipRect.x,
						(int)(fb_height - pcmd->ClipRect.w),
						(int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
						(int)(pcmd->ClipRect.w - pcmd->ClipRect.y));

					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
						sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}

		/* 恢复 OpenGL 原来的状态 */
		imguiOpenGLContext->PopOpenGLState();
	}

	const char* ImGuiOpenGLContext::GetClipboardText(void* user_data)
	{
		return glfwGetClipboardString(static_cast<GLFWwindow*>(user_data));
	}

	void ImGuiOpenGLContext::SetClipboardText(void* user_data, const char* text)
	{
		glfwSetClipboardString(static_cast<GLFWwindow*>(user_data), text);
	}

	void ImGuiOpenGLContext::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (action == GLFW_PRESS) {
			io.KeysDown[key] = true;
		}
		if (action == GLFW_RELEASE) {
			io.KeysDown[key] = false;
		}

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	}

	void ImGuiOpenGLContext::CharCallback(GLFWwindow* window, unsigned int c)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000) {
			io.AddInputCharacter((unsigned short)c);
		}
	}

	void ImGuiOpenGLContext::MouseButtonCallback(GLFWwindow* window, int button, int action, int modify)
	{
		if (action == GLFW_PRESS && button >= 0 && button < 3) {
			imguiOpenGLContext->mousePressed[button] = true;
		}
	}

	void ImGuiOpenGLContext::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		imguiOpenGLContext->fMouseWheel += (float)yoffset;
	}
}