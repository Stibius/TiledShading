#include <Renderer.h>
#include <PointLight.h>
#include <SimpleVT.h>
#include <Camera.h>

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
		m_visualizationTechnique = std::make_unique<SimpleVT>();
		m_visualizationTechnique->m_glContext = m_glContext;
	}

	if (m_shaderProgram == nullptr)
	{
		//load shaders
		std::string shaderDir(APP_RESOURCES"/shaders/");
		loadShaders(ge::core::loadTextFile(shaderDir + "VS.glsl"), ge::core::loadTextFile(shaderDir + "FS.glsl"));
		m_visualizationTechnique->m_shaderProgram = m_shaderProgram;
	}

	//setup light uniforms
	if (m_needToSetLightUniforms)
	{
		m_shaderProgram->set1i("lightCount", m_pointLights.size());

		if (m_pointLights.size() != 0)
		{
			m_lightsBuffer = std::make_unique<ge::gl::Buffer>(m_pointLights.size() * sizeof(ge::sg::PointLight), m_pointLights.data(), GL_DYNAMIC_DRAW);
			m_lightsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
		}

		m_needToSetLightUniforms = false;
	}

	m_shaderProgram->setMatrix4fv("projection", glm::value_ptr(m_camera->m_perspective->getProjection()));
	if (m_camera->m_orbit)
	{
		m_shaderProgram->setMatrix4fv("view", glm::value_ptr(m_camera->m_orbit->getView()));
		m_shaderProgram->set3fv("viewPos", glm::value_ptr(glm::inverse(m_camera->m_orbit->getView()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	}
	else if (m_camera->m_freeLook)
	{
		m_shaderProgram->setMatrix4fv("view", glm::value_ptr(m_camera->m_freeLook->getView()));
		m_shaderProgram->set3fv("viewPos", glm::value_ptr(glm::inverse(m_camera->m_freeLook->getView()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	}

	if (m_needToProcessScene)
	{
		std::shared_ptr<ge::glsg::GLScene> m_glScene = ge::glsg::GLSceneProcessor::processScene(m_scene, m_glContext);
		m_visualizationTechnique->setScene(m_glScene);
		m_visualizationTechnique->processScene();
		m_needToProcessScene = false;
	}

	setupGLState();

	m_glContext->glFinish();

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	m_visualizationTechnique->draw();

	m_glContext->glFinish();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<int64_t, std::micro> elapsed = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(t2 - t1);

	return elapsed.count();
}
