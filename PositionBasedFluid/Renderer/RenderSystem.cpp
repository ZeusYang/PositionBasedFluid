#include "RenderSystem.h"

#include "Camera/FPSCamera.h"
#include "Camera/TPSCamera.h"

#include <iostream>

using namespace std;

namespace Renderer
{
	void RenderSystem::resize(int width, int height)
	{
		m_width = width;
		m_height = height;
		// resize event.
		glViewport(0, 0, width, height);
		if(m_camera != nullptr)
			m_camera->changeAspect(static_cast<float>(width) / height);
	}

	void RenderSystem::initialize(int width, int height)
	{
		m_width = width;
		m_height = height;
		// initialization.
		resize(width, height);
		m_meshMgr = MeshMgr::getSingleton();
		m_shaderMgr = ShaderMgr::getSingleton();
		m_textureMgr = TextureMgr::getSingleton();
		m_drawableList = make_shared<DrawableList>();
		// load built-in shaders.
		m_shaderMgr->loadShader("shadow", "./glsl/depth.vert", "./glsl/depth.frag");
	}

	void RenderSystem::setSkyDome(const std::string & path, const std::string & pFix)
	{
		// set skydome background.
		if (m_skyDome != nullptr)
			return;
		unsigned int skyboxShader = m_shaderMgr->loadShader("skybox",
			"./glsl/skybox.vert", "./glsl/skybox.frag");
		unsigned int cubeTex = m_textureMgr->loadTextureCube("skybox", path, pFix);
		unsigned int mesh = m_meshMgr->loadMesh(new Sphere(1.0f, 10, 10));
		m_skyDome = make_shared<SkyDome>(skyboxShader);
		m_skyDome->addMesh(mesh, cubeTex);
	}

	Camera3D::ptr RenderSystem::createFPSCamera(glm::vec3 pos, glm::vec3 target)
	{
		// create a first person camera.
		FPSCamera *_cam = new FPSCamera(pos);
		_cam->lookAt(glm::normalize(target - pos));
		m_camera = shared_ptr<Camera3D>(_cam);
		return m_camera;
	}

	Camera3D::ptr RenderSystem::createTPSCamera(glm::vec3 pos, glm::vec3 target)
	{
		// create a third person camera.
		TPSCamera *_cam = new TPSCamera(target, 0.0f, 30.0f, 3.0f);
		m_camera = shared_ptr<Camera3D>(_cam);
		return m_camera;
	}

	void RenderSystem::createShadowDepthBuffer(int width, int height)
	{
		FrameBuffer *framebuf = new FrameBuffer(width, height, FrameBuffer::DepthBuffer);
		m_shadowDepthBuffer = shared_ptr<FrameBuffer>(framebuf);
	}

	void RenderSystem::createSunLightCamera(glm::vec3 target, float left, float right,
		float bottom, float top, float near, float far)
	{
		if (m_sunLight == nullptr)
		{
			std::cout << "You haven't create a light source.\n";
			return;
		}
		const float length = 200.0f;
		glm::vec3 pos = length * m_sunLight->getDirection();
		if (m_lightCamera == nullptr)
		{
			FPSCamera *cam = new FPSCamera(pos);
			m_lightCamera = shared_ptr<Camera3D>(cam);
		}
		m_lightCamera->setOrthographicProject(left, right, bottom, top, near, far);
		FPSCamera *cam = reinterpret_cast<FPSCamera*>(m_lightCamera.get());
		cam->lookAt(-m_sunLight->getDirection());
	}

	void RenderSystem::setClearMask(GLbitfield mask)
	{
		m_renderState.m_clearMask = mask;
	}

	void RenderSystem::setClearColor(glm::vec4 clearColor)
	{
		m_renderState.m_clearColor = clearColor;
	}

	void RenderSystem::setCullFace(bool enable, GLenum face)
	{
		m_renderState.m_cullFace = enable;
		m_renderState.m_cullFaceMode = face;
	}

	void RenderSystem::setDepthTest(bool enable, GLenum func)
	{
		m_renderState.m_depthTest = enable;
		m_renderState.m_depthFunc = func;
	}

	void RenderSystem::setSunLight(glm::vec3 dir, glm::vec3 amb, 
		glm::vec3 diff, glm::vec3 spec)
	{
		DirectionalLight *light = new DirectionalLight();
		light->setDirection(dir);
		light->setLightColor(amb, diff, spec);
		m_sunLight = shared_ptr<DirectionalLight>(light);
	}

	void RenderSystem::setPolygonMode(GLenum mode)
	{
		m_renderState.m_polygonMode = mode;
	}

	void RenderSystem::render()
	{
		if (m_drawableList == nullptr)
			return;

		// render the shadow.
		renderShadowDepth();

		// polygon mode.
		glPolygonMode(GL_FRONT_AND_BACK, m_renderState.m_polygonMode);

		// cullface setting.
		if (m_renderState.m_cullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		glCullFace(m_renderState.m_cullFaceMode);

		// depth testing setting.
		if (m_renderState.m_depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		glDepthFunc(m_renderState.m_depthFunc);

		// clearing color.
		glClearColor(m_renderState.m_clearColor.x, m_renderState.m_clearColor.y,
			m_renderState.m_clearColor.z, m_renderState.m_clearColor.w);
		glClear(m_renderState.m_clearMask);

		// render the drawable list.
		m_drawableList->render(m_camera, m_sunLight, m_lightCamera);

		// render the skydome.
		if (m_skyDome != nullptr)
		{
			glDepthFunc(GL_LEQUAL);
			glCullFace(GL_FRONT);
			m_skyDome->render(m_camera, m_sunLight, m_lightCamera);
		}
	}

	void RenderSystem::renderShadowDepth()
	{
		// render the shadow-depth map.
		if (m_lightCamera == nullptr || m_shadowDepthBuffer == nullptr)
			return;
		m_shadowDepthBuffer->bind();
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		m_drawableList->renderDepth(
			m_shaderMgr->getShader("shadow"), m_lightCamera);
		m_shadowDepthBuffer->unBind(m_width, m_height);
		glDisable(GL_CULL_FACE);
	}
}