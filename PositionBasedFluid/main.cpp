#include <iostream>

#include <time.h>
#include <thread>

#include "Renderer/RenderDevice.h"
#include "Renderer/Camera/TPSCamera.h"
#include "Renderer/Drawable/Geometry.h"
#include "Renderer/Drawable/InstanceDrawable.h"
#include "Renderer/Drawable/StaticModelDrawable.h"
#include "Renderer/Voxelization.h"

#include "Simulator/FluidModelSolver.h"

using namespace std;
using namespace Renderer;
using namespace Simulator;

// Simulator
FluidModelSolver fluid;
void buildModel();
void createBreakingDamScene();
void addWall(const glm::vec3 &minX, const glm::vec3 &maxX, std::vector<glm::vec3> &boundaryParticles);
void initBoundaryData(std::vector<glm::vec3> &boundaryParticles);

const real particleRadius = static_cast<real>(0.017);
const unsigned int fwidth = 15;
const unsigned int fdepth = 15;
const unsigned int fheight = 20;
const real containerWidth = (fwidth + 5)*particleRadius*static_cast<real>(4.0);
const real containerDepth = (fdepth + 5)*particleRadius*static_cast<real>(4.0);
const real containerHeight = 1.5;

// Particle render.
void pushParticlesToRender(
	InstanceDrawable *drawable,
	const std::vector<glm::vec3> &particles,
	const unsigned int &start,
	const unsigned int &end);

void runSimulation();

InstanceDrawable *renderParticles;
bool runIt = false;
bool showWall = false;

int main(int argc, char *argv[])
{
	// initialization.
	const int width = 1000, height = 800;
	RenderDevice::ptr window = RenderDevice::getSingleton();
	window->initialize("Position Based Fluid", width, height);
	RenderSystem::ptr renderSys = window->getRenderSystem();

	// resource loading.
	MeshMgr::ptr meshMgr = renderSys->getMeshMgr();
	ShaderMgr::ptr shaderMgr = renderSys->getShaderMgr();
	TextureMgr::ptr textureMgr = renderSys->getTextureMgr();
	// shaders.
	unsigned int simpleColorShader = shaderMgr->loadShader("simple_color",
		"./glsl/simple_color.vert", "./glsl/simple_color.frag");
	unsigned int simpleTextShader = shaderMgr->loadShader("simple_texture",
		"./glsl/simple_texture.vert", "./glsl/simple_texture.frag");
	unsigned int blinnPhongShader = shaderMgr->loadShader("blinn_phong",
		"./glsl/blinn_phong.vert", "./glsl/blinn_phong.frag");
	// textures.
	unsigned int tex1 = textureMgr->loadTexture2D("floor", "./res/floor.jpg");
	unsigned int cube1 = textureMgr->loadTexture2D("cube", "./res/cube.jpg");
	unsigned int blue = textureMgr->loadTexture2D("blue", "./res/blue.png");

	renderSys->setSkyDome("./res/skybox/", ".jpg");
	// meshes.
	unsigned int planeMesh = meshMgr->loadMesh(new Plane(500.0f, 500.0f));
	unsigned int cubeMesh = meshMgr->loadMesh(new Cube(1.0f, 1.0f, 1.0f));
	unsigned int sphereMesh1 = meshMgr->loadMesh(new Sphere(1.0f, 10, 10));
	unsigned int sphereMesh2 = meshMgr->loadMesh(new Sphere(0.5f, 10, 10));

	// sunlight.
	renderSys->setSunLight(glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.2f), glm::vec3(0.5f), glm::vec3(0.6f));

	// add drawable objects.
	Drawable *floor = new SimpleObject(blinnPhongShader);
	floor->addMesh(planeMesh);
	floor->addTexture(tex1);

	StaticModelDrawable *model = new StaticModelDrawable(blinnPhongShader,
		"./res/nanosuit/nanosuit.obj");
	Transform3D *trans = model->getTransformation();
	trans->scale(glm::vec3(0.2f));
	trans->translate(glm::vec3(2.5, 0.0, -2.0));

	StaticModelDrawable *robot = new StaticModelDrawable(simpleColorShader,
		"./res/bunny.obj");
	Transform3D *trans1 = robot->getTransformation();
	//trans1->scale(glm::vec3(0.8f));
	//trans1->translate(glm::vec3(-2.5, 1.0, +2.0));
	trans1->translate(glm::vec3(-1.0, 1.0, 2.0));

	Drawable *box = new SimpleObject(blinnPhongShader);
	box->addMesh(cubeMesh);
	box->addTexture(cube1);
	box->getTransformation()->translate(glm::vec3(-3.0f, 0.5f, -2.0f));

	InstanceDrawable * voxelVisual = new InstanceDrawable(blinnPhongShader);
	voxelVisual->addMesh(sphereMesh2);
	voxelVisual->addTexture(blue);
	voxelVisual->setReceiveShadow(false);
	Transform3D *voxelTrans = voxelVisual->getTransformation();
	voxelTrans->translate(glm::vec3(-1.0, 1.0, 2.0));

	renderParticles = new InstanceDrawable(blinnPhongShader);
	renderParticles->addMesh(sphereMesh1);
	renderParticles->addTexture(blue);
	renderParticles->setReceiveShadow(false);

	renderSys->addDrawable(floor);
	renderSys->addDrawable(model);
	renderSys->addDrawable(box);
	renderSys->addDrawable(robot);
	renderSys->addDrawable(voxelVisual);
	renderSys->addDrawable(renderParticles);

	//Camera3D::ptr camera = renderSys->createFPSCamera(glm::vec3(0, 0, 2), glm::vec3(0,0,0));
	Camera3D::ptr camera = renderSys->createTPSCamera(
		glm::vec3(0, 0, 0),
		glm::vec3(containerWidth * (-0.5) + fwidth * particleRadius * 0.5f,
			0.0f,
			containerHeight * (-0.5) + fdepth * particleRadius * 0.5f));
	camera->setPerspectiveProject(45.0f, static_cast<float>(width) / height, 0.1f, 100.0f);
	TPSCamera *tpsCamera = reinterpret_cast<TPSCamera*>(camera.get());
	tpsCamera->setPitch(15.0f);
	tpsCamera->setDistance(7.0f);

	// shadow setting.
	renderSys->createShadowDepthBuffer(2048, 2048);
	renderSys->createSunLightCamera(glm::vec3(0.0f), -25.0f, +25.0f,
		-25.0f, +25.0f, 1.0f, 300.0f);

	buildModel();
	unsigned int nParticles = fluid.getFluidParticleNum();
	unsigned int nWallParticles = fluid.getTotalParticleNum() - nParticles;
	pushParticlesToRender(renderParticles, fluid.getParticleGroup(), 0,
		nParticles + showWall * nWallParticles);

	// simulation thread
	thread simulationThread(runSimulation);
	simulationThread.detach();

	// voxelization
	Voxelization voxel;
	std::vector<glm::vec3> ret;
	std::vector<glm::mat4> inst;
	voxel.voxelize(robot, 0.1, ret);
	for (unsigned int x = 0; x < ret.size(); ++x)
	{
		glm::mat4 transf(1.0f);
		transf = glm::translate(transf, ret[x]);
		transf = glm::scale(transf, glm::vec3(0.1));
		inst.push_back(transf);
	}
	if(ret.size() > 0)
		voxelVisual->setInstanceMatrix(inst);
	robot->setVisiable(false);

	// renderer loop.
	while (window->run())
	{
		window->beginFrame();
		renderSys->render();

		if (runIt)
		{
			pushParticlesToRender(renderParticles, fluid.getParticleGroup(),
				0, nParticles + showWall * nWallParticles);
		}

		{
			ImGui::Begin("Simulator");
			ImGui::Text("Time step size: %.4f", fluid.getTimeStepSize());
			if (ImGui::Button("Reset"))
			{
				buildModel();
				pushParticlesToRender(renderParticles, fluid.getParticleGroup(),
					0, nParticles + showWall * nWallParticles);
				runIt = false;
			}
			if (ImGui::Button("Show walls"))
			{
				showWall = !showWall;
				pushParticlesToRender(renderParticles, fluid.getParticleGroup(),
						0, nParticles + showWall * nWallParticles);
			}
			if (ImGui::Button("Run"))
				runIt = !runIt;
			ImGui::End();
		}

		window->endFrame();
		//TimeManager::getSingleton()->setTimeStepSize(RenderDevice::m_deltaTime);
	}

	window->shutdown();

	return 0;
}

void buildModel()
{
	//TimeManager::getSingleton()->setTimeStepSize(0.0083);
	TimeManager::getSingleton()->setTimeStepSize(0.0025);
	//TimeManager::getSingleton()->setTimeStepSize(0.005);
	createBreakingDamScene();
}

void createBreakingDamScene()
{
	const real diam = 2.0 * particleRadius;
	const real startX = -0.5 * containerWidth + diam;
	const real startY = 10 * diam;
	const real startZ = -0.5 * containerDepth + diam;

	std::vector<glm::vec3> fluidParticles;
	fluidParticles.resize(fwidth*fheight*fdepth);
	for (int x = 0; x < fwidth; ++x)
	{
		for (int y = 0; y < fheight; ++y)
		{
			for (int z = 0; z < fdepth; ++z)
			{
				fluidParticles[x*fheight*fdepth + y * fdepth + z] =
					glm::vec3(startX, startY, startZ) +
					static_cast<float>(diam) * glm::vec3(x, y, z);
			}
		}
	}
	fluid.setParticleRadius(particleRadius);

	std::vector<glm::vec3> boundaryParticles;
	initBoundaryData(boundaryParticles);

	fluid.setupModel(fluidParticles, boundaryParticles);
}

void addWall(const glm::vec3 &minX, const glm::vec3 &maxX, std::vector<glm::vec3> &boundaryParticles)
{
	const real diam = 2.0 * fluid.getParticleRadius();
	const glm::vec3 diff = maxX - minX;
	const unsigned int stepsX = static_cast<unsigned int>(diff.x / diam) + 1;
	const unsigned int stepsY = static_cast<unsigned int>(diff.y / diam) + 1;
	const unsigned int stepsZ = static_cast<unsigned int>(diff.z / diam) + 1;
	
	for (unsigned int x = 0; x < stepsX; x++)
	{
		for (unsigned int y = 0; y < stepsY; y++)
		{
			for (unsigned int z = 0; z < stepsZ; z++)
			{
				const glm::vec3 currPos = minX + glm::vec3(x*diam, y*diam, z*diam);
				boundaryParticles.push_back(currPos);
			}
		}
	}
}

void initBoundaryData(std::vector<glm::vec3> &boundaryParticles)
{
	const real x1 = -containerWidth * 0.5;
	const real x2 = containerWidth * 0.5;
	const real y1 = 0.0;
	const real y2 = containerHeight;
	const real z1 = -containerDepth * 0.5;
	const real z2 = containerDepth * 0.5;

	// Floor
	addWall(glm::vec3(x1, y1, z1), glm::vec3(x2, y1, z2), boundaryParticles);
	// Top
	addWall(glm::vec3(x1, y2, z1), glm::vec3(x2, y2, z2), boundaryParticles);
	// Left
	addWall(glm::vec3(x1, y1, z1), glm::vec3(x1, y2, z2), boundaryParticles);
	// Right
	addWall(glm::vec3(x2, y1, z1), glm::vec3(x2, y2, z2), boundaryParticles);
	// Back
	addWall(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z1), boundaryParticles);
	// Front
	addWall(glm::vec3(x1, y1, z2), glm::vec3(x2, y2, z2), boundaryParticles);
}

void pushParticlesToRender(
	InstanceDrawable *drawable,
	const std::vector<glm::vec3> &particles,
	const unsigned int &start,
	const unsigned int &end)
{
	std::vector<glm::mat4> instanceMatrices;
	unsigned int nParticles = end - start;
	static bool test = true;
	if (test)
	{
		cout << "nParticles->" << nParticles << endl;
		test = false;
	}
	instanceMatrices.reserve(nParticles);
	//std::vector<glm::vec3> &particles = fluid.getParticleGroup();
	for (unsigned int x = start; x < end; ++x)
	{
		glm::mat4 matrix(1.0f);
		matrix = glm::translate(matrix, particles[x]);
		matrix = glm::scale(matrix, glm::vec3(particleRadius));
		instanceMatrices.push_back(matrix);
	}
	drawable->setInstanceMatrix(instanceMatrices, GL_DYNAMIC_DRAW);

}

void runSimulation()
{
	unsigned int nParticles = fluid.getFluidParticleNum();
	unsigned int nWallParticles = fluid.getTotalParticleNum() - nParticles;
	RenderDevice::ptr window = RenderDevice::getSingleton();
	while (window->run())
	{
		if (runIt)
		{
			fluid.simulate();
			fluid.simulate();
		}
	}
}