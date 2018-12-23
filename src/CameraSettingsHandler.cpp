#include <CameraSettingsHandler.h>
#include <Camera.h>
#include <Scene.h>
#include <AABB.h>

#include <geUtil/OrbitCamera.h>
#include <geUtil/FreeLookCamera.h>
#include <geUtil/PerspectiveCamera.h>

ts::CameraSettingsHandler::CameraSettingsHandler(Camera* camera, const Scene* scene)
	: m_camera(camera)
	, m_scene(scene)
{
}

void ts::CameraSettingsHandler::init(Camera* camera, const Scene* scene)
{
	m_camera = camera;
	m_scene = scene;
}

void ts::CameraSettingsHandler::setFovy(float value)
{
	if (value != getFovy())
	{
		m_camera->m_perspective->setFovy(glm::radians(value));

		emit fovyChanged(value);
		emit render();
	}
}

void ts::CameraSettingsHandler::setRelativeStepSize(float value)
{
	if (value != getRelativeStepSize())
	{
		m_relativeStepSize = value;

		emit relativeStepSizeChanged(value);
		emit absoluteStepSizeChanged(value * m_scene->getBoundingBox().getLongestSide());
	}
}

void ts::CameraSettingsHandler::setRotationSpeed(float value)
{
	if (value != getRotationSpeed())
	{
		m_rotationSpeed = value;

		emit rotationSpeedChanged(value);
	}
}

void ts::CameraSettingsHandler::setRelativeZoomSpeed(float value)
{
	if (value != getRelativeZoomSpeed())
	{
		m_relativeZoomSpeed = value;

		emit relativeZoomSpeedChanged(value);
		emit absoluteZoomSpeedChanged(value * m_scene->getBoundingBox().getLongestSide());
	}
}

void ts::CameraSettingsHandler::setCameraType(CameraType type)
{
	if (type != getCameraType())
	{
		switch (type)
		{
		case CameraType::ORBIT:
			m_camera->m_orbit = std::make_unique<ge::util::OrbitCamera>();
			m_camera->m_freeLook = nullptr;
			sceneChanged();
			break;
		case CameraType::FREELOOK:
			m_camera->m_orbit = nullptr;
			m_camera->m_freeLook = std::make_unique<ge::util::FreeLookCamera>();
			sceneChanged();
			break;
		default:
			break;
		}

		emit cameraTypeChanged(type);
		emit render();
	}
}

void ts::CameraSettingsHandler::sceneChanged()
{
	glm::vec3 center = m_scene->getBoundingBox().getCenter();
	float longestSide = m_scene->getBoundingBox().getLongestSide();
	float distance = (longestSide / 2) / (std::tan(m_camera->m_perspective->getFovy() / 2)) + (longestSide);

	m_camera->m_perspective->setFar((distance + longestSide) * 10.0f);

	if (m_camera->m_orbit)
	{
		m_camera->m_orbit->setFocus(-center);
		m_camera->m_orbit->setDistance(distance);
		m_camera->m_orbit->setAngles(glm::vec2(0.0f, 0.0f));
	}
	else if (m_camera->m_freeLook)
	{
		m_camera->m_freeLook->setXAngle(0.0f);
		m_camera->m_freeLook->setYAngle(0.0f);
		m_camera->m_freeLook->setZAngle(0.0f);
		m_camera->m_freeLook->setRotation(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
		m_camera->m_freeLook->setPosition(center);
		m_camera->m_freeLook->back(distance);
	}

	emit absoluteStepSizeChanged(m_relativeStepSize * m_scene->getBoundingBox().getLongestSide());
	emit absoluteZoomSpeedChanged(m_relativeZoomSpeed * m_scene->getBoundingBox().getLongestSide());
}

float ts::CameraSettingsHandler::getFovy() const
{
	return glm::degrees(m_camera->m_perspective->getFovy());
}

float ts::CameraSettingsHandler::getRelativeStepSize() const
{
	return m_relativeStepSize;
}

float ts::CameraSettingsHandler::getRotationSpeed() const
{
	return m_rotationSpeed;
}

float ts::CameraSettingsHandler::getRelativeZoomSpeed() const
{
	return m_relativeZoomSpeed;
}

ts::CameraSettingsHandler::CameraType ts::CameraSettingsHandler::getCameraType() const
{
	if (m_camera->m_freeLook)
	{
		return CameraType::FREELOOK;
	}
	else 
	{
		return CameraType::ORBIT;
	}
}
