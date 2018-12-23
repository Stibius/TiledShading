#pragma once

#include <QObject>

namespace ts
{
	class Camera;

	class KeyEventHandler : public QObject
	{
		Q_OBJECT

	public:

		KeyEventHandler() = default;

		KeyEventHandler(Camera* camera);

		virtual ~KeyEventHandler() = default;

		void init(Camera* camera);

	signals:

		void render();

	public slots:

		void absoluteStepSizeChanged(float value);

	protected:

		bool eventFilter(QObject* obj, QEvent* event);

		Camera* m_camera = nullptr;

		float m_absoluteStepSize = 0.1f;
	};
}

