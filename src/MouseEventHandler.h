#pragma once

#include <QObject>
#include <QPointF>

namespace ts
{
	class Camera;

	class MouseEventHandler : public QObject
	{
		Q_OBJECT

	public:

		MouseEventHandler() = default;

		MouseEventHandler(Camera* camera);

		virtual ~MouseEventHandler() = default;

		void init(Camera* camera);

	signals:

		void render();

	public slots:

		void absoluteZoomSpeedChanged(float value);

		void rotationSpeedChanged(float value);

	protected:

		bool eventFilter(QObject* obj, QEvent* event);

		Camera* m_camera = nullptr;
		bool m_rightMousePressed = false;
		bool m_leftMousePressed = false;
		QPointF m_lastLeftMousePos;
		QPointF m_lastRightMousePos;

		float m_rotationSpeed = 0.5f;
		float m_absoluteZoomSpeed = 0.1f;

	};
}
