#include <MouseEventHandler.h>
#include <Camera.h>
#include <Scene.h>
#include <AABB.h>

#include <geUtil/FreeLookCamera.h>
#include <geUtil/OrbitCamera.h>

#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

ts::MouseEventHandler::MouseEventHandler(Camera* camera)
	: m_camera(camera)
{
}

void ts::MouseEventHandler::init(Camera* camera)
{
	m_camera = camera;
}

void ts::MouseEventHandler::absoluteZoomSpeedChanged(float value)
{
	m_absoluteZoomSpeed = value;
}

void ts::MouseEventHandler::rotationSpeedChanged(float value)
{
	m_rotationSpeed = value;
}

bool ts::MouseEventHandler::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress) 
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() == Qt::LeftButton)
		{
			m_leftMousePressed = true;
			m_lastLeftMousePos = mouseEvent->pos();

			return true;
		}
		else if (mouseEvent->button() == Qt::RightButton)
		{
			m_rightMousePressed = true;
			m_lastRightMousePos = mouseEvent->pos();

			return true;
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease) 
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() == Qt::LeftButton)
		{
			m_leftMousePressed = false;

			return true;
		}
		else if (mouseEvent->button() == Qt::RightButton)
		{
			m_rightMousePressed = false;

			return true;
		}
	}
	else if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		bool change = false;

		if (m_leftMousePressed)
		{
			QPointF diff = mouseEvent->pos() - m_lastLeftMousePos;
			m_lastLeftMousePos = mouseEvent->pos();

			if (m_camera->m_orbit)
			{
				m_camera->m_orbit->addXAngle(glm::radians(diff.y()) * m_rotationSpeed);
				m_camera->m_orbit->addYAngle(glm::radians(diff.x()) * m_rotationSpeed);
				change = true;
			}
			else if (m_camera->m_freeLook)
			{
				m_camera->m_freeLook->addXAngle(glm::radians(diff.y()) * m_rotationSpeed);
				m_camera->m_freeLook->addYAngle(glm::radians(diff.x()) * m_rotationSpeed);
				change = true;
			}
		}
		
		if (m_rightMousePressed)
		{
			float diff = mouseEvent->pos().y() - m_lastRightMousePos.y();
			m_lastRightMousePos = mouseEvent->pos();

			if (m_camera->m_orbit)
			{
				m_camera->m_orbit->addDistance(diff * m_absoluteZoomSpeed);
				change = true;
			}
		}

		if (change) emit render();

		return true;
		
	}

	return QObject::eventFilter(obj, event);
}
