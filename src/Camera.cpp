
#include <Camera.h>

#include <geUtil/PerspectiveCamera.h>
#include <geUtil/OrbitCamera.h>
#include <geUtil/FreeLookCamera.h>

ts::Camera::Camera()
{
	m_perspective->setFovy(glm::radians(45.0f));
}

ts::Camera::Camera(const Camera& camera)
{
	if (camera.m_perspective)
	{
		m_perspective = std::make_unique<ge::util::PerspectiveCamera>(*camera.m_perspective);
	}
	else
	{
		m_perspective = nullptr;
	}

	if (camera.m_orbit)
	{
		m_orbit = std::make_unique<ge::util::OrbitCamera>(*camera.m_orbit);
	}
	else
	{
		m_orbit = nullptr;
	}

	if (camera.m_freeLook)
	{
		m_freeLook = std::make_unique<ge::util::FreeLookCamera>(*camera.m_freeLook);
	}
	else
	{
		m_freeLook = nullptr;
	}
}

ts::Camera& ts::Camera::operator=(const Camera& camera)
{
	if (this == &camera)
	{
		return *this;
	}

	if (camera.m_perspective)
	{
		m_perspective = std::make_unique<ge::util::PerspectiveCamera>(*camera.m_perspective);
	}
	else
	{
		m_perspective = nullptr;
	}

	if (camera.m_orbit)
	{
		m_orbit = std::make_unique<ge::util::OrbitCamera>(*camera.m_orbit);
	}
	else
	{
		m_orbit = nullptr;
	}

	if (camera.m_freeLook)
	{
		m_freeLook = std::make_unique<ge::util::FreeLookCamera>(*camera.m_freeLook);
	}
	else
	{
		m_freeLook = nullptr;
	}

	return *this;
}
