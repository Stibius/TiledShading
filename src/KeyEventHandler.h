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

		explicit KeyEventHandler(Camera* camera);

		KeyEventHandler(const KeyEventHandler& other) = default;

		KeyEventHandler(KeyEventHandler&& other) = default;

		KeyEventHandler& operator=(const KeyEventHandler& other) = default;

		KeyEventHandler& operator=(KeyEventHandler&& other) = default;

		~KeyEventHandler() override = default;

		void init(Camera* camera);

		bool eventFilter(QObject* obj, QEvent* event) override;

	signals:

		void render();

	public slots:

		void absoluteStepSizeChanged(float value);

	protected:

		Camera* m_camera = nullptr;

		float m_absoluteStepSize = 0.1f;
	};
}

