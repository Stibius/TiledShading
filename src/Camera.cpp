
#include <Camera.h>

#include <geUtil/PerspectiveCamera.h>
#include <geUtil/OrbitCamera.h>
#include <geUtil/FreeLookCamera.h>

ts::Camera::Camera()
{
	m_perspective->setFovy(glm::radians(45.0f));
}

ts::Camera::Camera(const Camera& other)
{
	if (other.m_perspective)
	{
		m_perspective = std::make_unique<ge::util::PerspectiveCamera>(*other.m_perspective);
	}
	else
	{
		m_perspective = nullptr;
	}

	if (other.m_orbit)
	{
		m_orbit = std::make_unique<ge::util::OrbitCamera>(*other.m_orbit);
	}
	else
	{
		m_orbit = nullptr;
	}

	if (other.m_freeLook)
	{
		m_freeLook = std::make_unique<ge::util::FreeLookCamera>(*other.m_freeLook);
	}
	else
	{
		m_freeLook = nullptr;
	}
}

ts::Camera& ts::Camera::operator=(const Camera& other)
{
	if (this == &other)
	{
		return *this;
	}

	if (other.m_perspective)
	{
		m_perspective = std::make_unique<ge::util::PerspectiveCamera>(*other.m_perspective);
	}
	else
	{
		m_perspective = nullptr;
	}

	if (other.m_orbit)
	{
		m_orbit = std::make_unique<ge::util::OrbitCamera>(*other.m_orbit);
	}
	else
	{
		m_orbit = nullptr;
	}

	if (other.m_freeLook)
	{
		m_freeLook = std::make_unique<ge::util::FreeLookCamera>(*other.m_freeLook);
	}
	else
	{
		m_freeLook = nullptr;
	}

	return *this;
}
