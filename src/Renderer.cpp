#include <Renderer.h>
#include <QtQuick/qquickwindow.h>
#include <geGL/geGL.h>
#include <geCore/Text.h>
#include <geSG/Scene.h>
#include <geSG/AABB.h>
#include <geSG/Light.h>
#include <geutil/FreeLookCamera.h>
#include <geutil/OrbitCamera.h>
#include <geutil/PerspectiveCamera.h>
#include <glsg/GLSceneProcessor.h>
#include <QElapsedTimer>
#include <SimpleVT.h>
#include <SceneAABB.h>
#include <algorithm>
#include <stdlib.h>

Renderer::Renderer()
{
	m_transformCamera = std::make_unique<ge::util::OrbitCamera>();
	m_perspectiveCamera = std::make_unique<ge::util::PerspectiveCamera>();
	m_boundingBox = std::make_unique<ge::sg::SceneAABB>();
}

void Renderer::setViewportSize(const QSize& size)
{
	m_viewportSize = size;
	m_perspectiveCamera->setAspect(static_cast<float>(m_viewportSize.width()) / static_cast<float>(m_viewportSize.height()));
	m_perspectiveCamera->setFovy(glm::radians(m_fovy));
	m_perspectiveCamera->setNear(m_near);
	m_perspectiveCamera->setFar(m_far);
}

void Renderer::setWindow(QQuickWindow* window)
{
	m_window = window;
}

void Renderer::setupGLState()
{
	if (m_glContext == nullptr)
	{
		return;
	}

	m_glContext->glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
	m_glContext->glClearColor(0.4, 0.4, 0.4, 1.0);
	m_glContext->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::loadShaders(const std::string & vsPath, const std::string & fsPath)
{
	std::shared_ptr<ge::gl::Shader> simple_vs(std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, vsPath));
	std::shared_ptr<ge::gl::Shader> simple_fs(std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, fsPath));
	m_shaderProgram = std::make_shared<ge::gl::Program>(simple_vs, simple_fs);
}

void Renderer::setScene(std::shared_ptr<ge::sg::Scene> scene)
{
	m_scene = scene;
    m_boundingBox->compute(*m_scene);
	m_needToProcessScene = true;
	resetCamera();
	emit redraw();
}

float Renderer::getFovy() const
{
	return m_fovy;
}

int Renderer::getLightCount() const
{
	return m_lights.size();
}

float Renderer::getLightPosRange() const
{
	return m_lightPosRange;
}

float Renderer::getMovementSpeed() const
{
	return m_movementSpeedCoef;
}

Renderer::CameraType Renderer::getCameraType() const
{
	return m_cameraType;
}

void Renderer::setFovy(float value)
{
	m_fovy = value;
	m_perspectiveCamera->setFovy(glm::radians(m_fovy));

	emit redraw();
}

void Renderer::setMovementSpeed(float value)
{
	m_movementSpeedCoef = value;
}

void Renderer::setLightPosRange(float value)
{
	m_lightPosRange = value;
}

void Renderer::generateLights(int count)
{
	m_lights.clear();
	for (int i = 0; i < count; i++)
	{
		ge::sg::Light light;
		light.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		light.position.x = (m_boundingBox->min.x + (rand() / (RAND_MAX / (m_boundingBox->max.x - m_boundingBox->min.x)))) * m_lightPosRange;
		light.position.y = (m_boundingBox->min.y + (rand() / (RAND_MAX / (m_boundingBox->max.y - m_boundingBox->min.y)))) * m_lightPosRange;
		light.position.z = (m_boundingBox->min.z + (rand() / (RAND_MAX / (m_boundingBox->max.z - m_boundingBox->min.z)))) * m_lightPosRange;
		light.position.w = 1.0f;
		m_lights.push_back(light);
	}

	m_needToSetLightUniforms = true;

	emit redraw();
}

void Renderer::setCameraType(CameraType type)
{
	if (m_cameraType != type && type == CameraType::ORBIT)
	{
		m_cameraType = type;
		m_transformCamera = std::make_unique<ge::util::OrbitCamera>();
		resetCamera();
	}
	else if (m_cameraType != type && type == CameraType::FREELOOK)
	{
		m_cameraType = type;
		m_transformCamera = std::make_unique<ge::util::FreeLookCamera>();
		resetCamera();
	}
}

void Renderer::onMouseLeftPressed(const QPointF& position)
{
	m_mouseLeftPressed = true;
	m_lastLeftMousePos = glm::vec2(position.x(), position.y());
}

void Renderer::onMouseRightPressed(const QPointF& position)
{
	m_mouseRightPressed = true;
	m_lastRightMousePos = glm::vec2(position.x(), position.y());
}

void Renderer::onMouseLeftReleased(const QPointF& position)
{
	m_mouseLeftPressed = false;
}

void Renderer::onMouseRightReleased(const QPointF& position)
{
	m_mouseRightPressed = false;
}

void Renderer::onMousePositionChanged(const QPointF& position)
{
	glm::vec2 pos = glm::vec2(position.x(), position.y());

	ge::util::OrbitCamera* orbitCamera = dynamic_cast<ge::util::OrbitCamera*>(m_transformCamera.get());
	if (orbitCamera != nullptr)
	{
		if (m_mouseLeftPressed)
		{
			glm::vec2 diff = pos - m_lastLeftMousePos;
			orbitCamera->addXAngle(glm::radians(diff.y) * m_rotationSpeedCoef);
			orbitCamera->addYAngle(glm::radians(diff.x) * m_rotationSpeedCoef);
			m_lastLeftMousePos = pos;
		}

		if (m_mouseRightPressed)
		{
			float diff = pos.y - m_lastRightMousePos.y;
			orbitCamera->addDistance(diff * m_movementSpeedCoef * m_boundingBox->getLongestSide());
			m_lastRightMousePos = pos;
		}

		emit redraw();

		return;
	}

	ge::util::FreeLookCamera* freeLookCamera = dynamic_cast<ge::util::FreeLookCamera*>(m_transformCamera.get());
	if (freeLookCamera != nullptr)
	{
		if (m_mouseLeftPressed)
		{
			glm::vec2 diff = pos - m_lastLeftMousePos;
			freeLookCamera->addXAngle(glm::radians(diff.y) * m_rotationSpeedCoef);
			freeLookCamera->addYAngle(glm::radians(diff.x) * m_rotationSpeedCoef);
			m_lastLeftMousePos = pos;
		}

		emit redraw();
	}
}

void Renderer::onKeyPressed(Qt::Key key)
{
	ge::util::FreeLookCamera* freeLookCamera = dynamic_cast<ge::util::FreeLookCamera*>(m_transformCamera.get());
	if (freeLookCamera == nullptr)
	{
		return;
	}

	switch (key)
	{
	case Qt::Key::Key_W:
		freeLookCamera->forward(m_movementSpeedCoef * m_boundingBox->getLongestSide());
		break;
	case Qt::Key::Key_S:
		freeLookCamera->back(m_movementSpeedCoef * m_boundingBox->getLongestSide());
		break;
	case Qt::Key::Key_A:
		freeLookCamera->left(m_movementSpeedCoef * m_boundingBox->getLongestSide());
		break;
	case Qt::Key::Key_D:
		freeLookCamera->right(m_movementSpeedCoef * m_boundingBox->getLongestSide());
		break;
	case Qt::Key::Key_Space:
		freeLookCamera->up(m_movementSpeedCoef * m_boundingBox->getLongestSide());
		break;
	case Qt::Key::Key_C:
		freeLookCamera->down(m_movementSpeedCoef * m_boundingBox->getLongestSide());
		break;
	default:
		return;
	}

	emit redraw();
}

void Renderer::resetCamera()
{
	glm::vec3 center = m_boundingBox->getCenter();
	float longestSide = m_boundingBox->getLongestSide();
	float distance = (longestSide / 2) / (std::tan(glm::radians(m_fovy / 2))) + (longestSide);

	m_far = (distance + longestSide) * 10.0f;
	m_perspectiveCamera->setFar(m_far);

	ge::util::OrbitCamera* orbitCamera = dynamic_cast<ge::util::OrbitCamera*>(m_transformCamera.get());
	if (orbitCamera != nullptr)
	{
		orbitCamera->setFocus(-center);
		orbitCamera->setDistance(distance);
		orbitCamera->setAngles(glm::vec2(0.0f, 0.0f));

		emit redraw();

		return;
	}

	ge::util::FreeLookCamera* freeLookCamera = dynamic_cast<ge::util::FreeLookCamera*>(m_transformCamera.get());
	if (freeLookCamera != nullptr)
	{
		freeLookCamera->setXAngle(0.0f);
		freeLookCamera->setYAngle(0.0f);
		freeLookCamera->setZAngle(0.0f);
		freeLookCamera->setRotation(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
		freeLookCamera->setPosition(center);
		freeLookCamera->back(distance);

		emit redraw();
	}
}

void Renderer::onRender()
{
	if (m_glContext == nullptr)
	{
		//init geGL gl context
		ge::gl::init();
		m_glContext = std::make_shared<ge::gl::Context>();
		m_VT = std::make_unique<SimpleVT>();
		m_VT->m_glContext = m_glContext;
	}

	if (m_shaderProgram == nullptr)
	{
		//load shaders
		std::string shaderDir(APP_RESOURCES"/shaders/");
		loadShaders(ge::core::loadTextFile(shaderDir + "VS.glsl"), ge::core::loadTextFile(shaderDir + "FS.glsl"));
		m_VT->m_shaderProgram = m_shaderProgram;
	}

	//setup light uniforms
	if (m_needToSetLightUniforms)
	{
		m_shaderProgram->set1i("lightCount", m_lights.size());

		if (m_lights.size() != 0)
		{
			m_lightsBuffer = std::make_unique<ge::gl::Buffer>(m_lights.size() * sizeof(ge::sg::Light), m_lights.data(), GL_DYNAMIC_DRAW);
			m_lightsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
		}

		m_needToSetLightUniforms = false;
	}
	
	m_shaderProgram->setMatrix4fv("projection", glm::value_ptr(m_perspectiveCamera->getProjection()));
	m_shaderProgram->setMatrix4fv("view", glm::value_ptr(m_transformCamera->getView()));
	m_shaderProgram->set3fv("viewPos", glm::value_ptr(glm::inverse(m_transformCamera->getView()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	
	if (m_needToProcessScene)
	{
		std::shared_ptr<ge::glsg::GLScene> m_glScene = ge::glsg::GLSceneProcessor::processScene(std::move(m_scene), m_glContext);
		m_VT->setScene(m_glScene);
		m_VT->processScene();
		m_needToProcessScene = false;
	}

	setupGLState();

	m_glContext->glFinish();

	QElapsedTimer timer;
	timer.start();

	m_VT->draw();

	m_glContext->glFinish();

	static bool firstDraw = true; // updating rendering time in the GUI causes another redraw, only do it once
	if (firstDraw) emit renderingFinished(static_cast<float>(timer.nsecsElapsed()) / 1000000);
	firstDraw = !firstDraw;

	// Not strictly needed for this example, but generally useful for when
	// mixing with raw OpenGL.
	m_window->resetOpenGLState();
}
