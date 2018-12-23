#include <KeyEventHandler.h>
#include <Camera.h>

#include <geUtil/FreeLookCamera.h>

#include <QEvent>
#include <QKeyEvent>

ts::KeyEventHandler::KeyEventHandler(Camera* camera)
	: m_camera(camera)
{
}

void ts::KeyEventHandler::init(Camera* camera)
{
	m_camera = camera;
}

void ts::KeyEventHandler::absoluteStepSizeChanged(float value)
{
	m_absoluteStepSize = value;
}

bool ts::KeyEventHandler::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

		if (m_camera->m_freeLook)
		{
			switch (keyEvent->key())
			{
			case Qt::Key_W:

				m_camera->m_freeLook->forward(m_absoluteStepSize);
				emit render();
				return true;
			case Qt::Key_S:
				m_camera->m_freeLook->back(m_absoluteStepSize);
				emit render();
				return true;
			case Qt::Key_A:
				m_camera->m_freeLook->left(m_absoluteStepSize);
				emit render();
				return true;
			case Qt::Key_D:
				m_camera->m_freeLook->right(m_absoluteStepSize);
				emit render();
				return true;
			case Qt::Key_C:
				m_camera->m_freeLook->down(m_absoluteStepSize);
				emit render();
				return true;
			case Qt::Key_Space:
				m_camera->m_freeLook->up(m_absoluteStepSize);
				emit render();
				return true;
			default:
				break;
			}
		}
	}

	return QObject::eventFilter(obj, event);
}
