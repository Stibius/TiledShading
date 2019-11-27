#include <Renderer.h>
#include <PointLight.h>
#include <Camera.h>
#include <ForwardShadingVT.h>
#include <SceneVT.h>
#include <LightedSceneVT.h>
#include <FileLoader.h>

#include <geGL/geGL.h>
#include <geSG/Scene.h>
#include <geCore/Text.h>
#include <geutil/FreeLookCamera.h>
#include <geutil/OrbitCamera.h>
#include <geutil/PerspectiveCamera.h>
#include <glsg/GLSceneProcessor.h>

#include <chrono>

ts::Renderer::Renderer()
{
	m_noLightVT = std::make_unique<ForwardShadingVT>();
	std::string vsSource = FileLoader::loadFile(":/shaders/forwardVS.glsl");
	std::string fsSource = FileLoader::loadFile(":/shaders/forwardFS.glsl");
	m_noLightVT->setShaders(vsSource, fsSource);
}

void ts::Renderer::setViewportSize(int width, int height)
{
	m_viewPortWidth = width;
	m_viewPortHeight = height;
	m_camera->m_perspective->setAspect(static_cast<float>(m_viewPortWidth) / static_cast<float>(m_viewPortHeight));
	m_camera->m_perspective->setAspect(static_cast<float>(m_viewPortWidth) / static_cast<float>(m_viewPortHeight));
	m_VT->setViewportSize(width, height);
	m_noLightVT->setViewportSize(width, height);
}

void ts::Renderer::setScene(const ge::sg::Scene& scene)
{
	m_scene = std::make_shared<ge::sg::Scene>(scene);
	m_needToProcessScene = true;
}

void ts::Renderer::setupGLState()
{
	if (m_glContext == nullptr)
	{
		return;
	}

	m_glContext->glViewport(0, 0, m_viewPortWidth, m_viewPortHeight);
	m_glContext->glClearColor(0.0, 0.0, 0.0, 1.0);
	m_glContext->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ts::Renderer::setVisualizationTechnique(std::unique_ptr<SceneVT> visualizationTechnique)
{
	m_VT = std::move(visualizationTechnique);
	m_needToInitVT = true;
}

void ts::Renderer::setLights(std::shared_ptr<std::vector<ge::sg::PointLight>> pointLights)
{
	m_pointLights = pointLights;
	m_needToSetLightUniforms = true;
}

void ts::Renderer::setCamera(std::shared_ptr<Camera> camera)
{
	m_camera = camera;
}

int ts::Renderer::render()
{
	if (m_glContext == nullptr)
	{
		//init geGL gl context
		ge::gl::init();
		m_glContext = std::make_shared<ge::gl::Context>();
	}

	setupGLState();

	SceneVT* currentVT;
	if (!m_pointLights || m_pointLights->empty())
	{
		currentVT = m_noLightVT.get();
	}
	else
	{
		currentVT = m_VT.get();
	}

	if (m_needToInitVT)
	{
		m_VT->m_glContext = m_glContext;
		if (!m_needToProcessScene) m_VT->setScene(m_glScene);

		m_noLightVT->m_glContext = m_glContext;

		m_needToInitVT = false;
		m_needToSetLightUniforms = true;
	}

	if (m_needToProcessScene)
	{
		m_glScene = ge::glsg::GLSceneProcessor::processScene(m_scene, m_glContext);
		m_VT->setScene(m_glScene);
		m_noLightVT->setScene(m_glScene);
		m_needToProcessScene = false;
	}

	if (m_needToSetLightUniforms)
	{
		if (LightedSceneVT* lightVT = dynamic_cast<LightedSceneVT*>(m_VT.get()))
		{
			lightVT->setLights(m_pointLights);
		}

		m_needToSetLightUniforms = false;
	}

	currentVT->setProjectionMatrix(m_camera->m_perspective->getProjection());
	if (m_camera->m_orbit)
	{
		currentVT->setViewMatrix(m_camera->m_orbit->getView());
	}
	else if (m_camera->m_freeLook)
	{
		currentVT->setViewMatrix(m_camera->m_freeLook->getView());
	}
	currentVT->setModelMatrix(glm::mat4(1.0));

	currentVT->drawSetup();

	m_glContext->glFinish();

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	currentVT->draw();

	m_glContext->glFinish();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<int64_t, std::micro> elapsed = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(t2 - t1);

	return elapsed.count();
}
