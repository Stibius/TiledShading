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

		explicit MouseEventHandler(Camera* camera);

		MouseEventHandler(const MouseEventHandler& other) = default;

		MouseEventHandler(MouseEventHandler&& other) = default;

		MouseEventHandler& operator=(const MouseEventHandler& other) = default;

		MouseEventHandler& operator=(MouseEventHandler&& other) = default;

		~MouseEventHandler() override = default;

		void init(Camera* camera);

		bool eventFilter(QObject* obj, QEvent* event) override;

	signals:

		void render();

	public slots:

		void absoluteZoomSpeedChanged(float value);

		void rotationSpeedChanged(float value);

	protected:

		Camera* m_camera = nullptr;
		bool m_rightMousePressed = false;
		bool m_leftMousePressed = false;
		QPointF m_lastLeftMousePos;
		QPointF m_lastRightMousePos;

		float m_rotationSpeed = 0.5f;
		float m_absoluteZoomSpeed = 0.1f;

	};
}
