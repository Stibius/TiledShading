#pragma once

#include <QObject>

namespace ts
{
	class Camera;
	class Scene;

	class CameraSettingsHandler : public QObject
	{
		Q_OBJECT

	public:

		Q_PROPERTY(float fovy READ getFovy WRITE setFovy NOTIFY fovyChanged)
		Q_PROPERTY(float relativeStepSize READ getRelativeStepSize WRITE setRelativeStepSize NOTIFY relativeStepSizeChanged)
		Q_PROPERTY(float rotationSpeed READ getRotationSpeed WRITE setRotationSpeed NOTIFY rotationSpeedChanged)
		Q_PROPERTY(float relativeZoomSpeed READ getRelativeZoomSpeed WRITE setRelativeZoomSpeed NOTIFY relativeZoomSpeedChanged)
		Q_PROPERTY(CameraType cameraType READ getCameraType WRITE setCameraType NOTIFY cameraTypeChanged)

		enum class CameraType { ORBIT, FREELOOK };
		Q_ENUM(CameraType)

		CameraSettingsHandler() = default;

		CameraSettingsHandler(Camera* camera, const Scene* scene);

		virtual ~CameraSettingsHandler() = default;

		void init(Camera* camera, const Scene* scene);

		void setFovy(float value);

		void setRelativeStepSize(float value);

		void setRotationSpeed(float value);

		void setRelativeZoomSpeed(float value);

		void setCameraType(CameraType type);

		float getFovy() const;

		float getRelativeStepSize() const;

		float getRotationSpeed() const;

		float getRelativeZoomSpeed() const;

		CameraType getCameraType() const;

	signals:

		void fovyChanged(float value);

		void relativeStepSizeChanged(float value);

		void absoluteStepSizeChanged(float value);

		void rotationSpeedChanged(float value);

		void relativeZoomSpeedChanged(float value);

		void absoluteZoomSpeedChanged(float value);

		void cameraTypeChanged(CameraType type);

		void render();

	public slots:

		void sceneChanged();

	protected:

		Camera* m_camera = nullptr;
		const Scene* m_scene = nullptr;

		float m_relativeStepSize = 0.1f;
		float m_relativeZoomSpeed = 0.1f;
		float m_rotationSpeed = 0.5f;
	};
}
