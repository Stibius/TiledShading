#include <Renderer.h>
#include <PointLight.h>
#include <Camera.h>
#include <LightedSceneVT.h>
#include <PhongVT.h>

#include <geGL/geGL.h>
#include <geCore/Text.h>
#include <geSG/Scene.h>
#include <geutil/FreeLookCamera.h>
#include <geutil/OrbitCamera.h>
#include <geutil/PerspectiveCamera.h>
#include <glsg/GLSceneProcessor.h>

#include <chrono>

ts::Renderer::Renderer()
{
}

void ts::Renderer::setViewportSize(int width, int height)
{
	m_viewPortWidth = width;
	m_viewPortHeight = height;
	m_camera->m_perspective->setAspect(static_cast<float>(m_viewPortWidth) / static_cast<float>(m_viewPortHeight));
	m_camera->m_perspective->setAspect(static_cast<float>(m_viewPortWidth) / static_cast<float>(m_viewPortHeight));
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
	m_glContext->glClearColor(0.4, 0.4, 0.4, 1.0);
	m_glContext->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ts::Renderer::loadShaders(const std::string& vsPath, const std::string& fsPath)
{
	std::shared_ptr<ge::gl::Shader> simple_vs(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, vsPath));
	std::shared_ptr<ge::gl::Shader> simple_fs(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, fsPath));
	m_shaderProgram = std::make_shared<ge::gl::Program>(simple_vs, simple_fs);
}

void ts::Renderer::setVisualizationTechnique(std::unique_ptr<LightedSceneVT> visualizationTechnique)
{
	m_lightedSceneVT = std::move(visualizationTechnique);
	m_needToInitVT = true;
}

void ts::Renderer::setLights(const std::vector<ge::sg::PointLight>& pointLights)
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

	//if (m_lightedSceneVT == nullptr)
	//{
	//return 0;
	//}

	if (m_shaderProgram == nullptr)
	{
		//load shaders
		std::string shaderDir(APP_RESOURCES"/shaders/");
		loadShaders(ge::core::loadTextFile(shaderDir + "VS.glsl"), ge::core::loadTextFile(shaderDir + "FS.glsl"));
	}

	std::unique_ptr<PhongVT> phongVT = std::make_unique<PhongVT>();
	phongVT->m_shaderProgram = m_shaderProgram;
	setVisualizationTechnique(std::move(phongVT));

	if (m_needToInitVT)
	{
		m_lightedSceneVT->m_glContext = m_glContext;
		if (!m_needToProcessScene) m_lightedSceneVT->setScene(m_glScene);
		if (!m_needToSetLightUniforms) m_lightedSceneVT->setLights(m_pointLights);
		m_needToInitVT = false;
	}

	if (m_needToProcessScene)
	{
		m_glScene = ge::glsg::GLSceneProcessor::processScene(m_scene, m_glContext);
		m_lightedSceneVT->setScene(m_glScene);
		m_needToProcessScene = false;
	}

	if (m_needToSetLightUniforms)
	{
		m_lightedSceneVT->setLights(m_pointLights);
		m_needToSetLightUniforms = false;
	}

	m_lightedSceneVT->setProjectionMatrix(m_camera->m_perspective->getProjection());
	if (m_camera->m_orbit)
	{
		m_lightedSceneVT->setViewMatrix(m_camera->m_orbit->getView());
	}
	else if (m_camera->m_freeLook)
	{
		m_lightedSceneVT->setViewMatrix(m_camera->m_freeLook->getView());
	}

	m_glContext->glFinish();

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	m_lightedSceneVT->draw();

	m_glContext->glFinish();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<int64_t, std::micro> elapsed = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(t2 - t1);

	return elapsed.count();
}
