#pragma once

#include <QtQuick/QQuickItem>
#include <memory>

class Renderer;
class QThread;
class SceneLoader;

namespace ge
{
	namespace sg
	{
		class Scene;
	}
}

class RendererQQuickItem : public QQuickItem
{
	Q_OBJECT

public:

	Q_PROPERTY(bool sceneLoading MEMBER m_sceneLoading NOTIFY sceneLoadingChanged)
	Q_PROPERTY(float renderTime MEMBER m_renderTime NOTIFY renderTimeChanged)
	Q_PROPERTY(int fovy READ getFovy WRITE setFovy NOTIFY fovyChanged)
	Q_PROPERTY(int movementSpeed READ getMovementSpeed WRITE setMovementSpeed NOTIFY movementSpeedChanged)
	Q_PROPERTY(CameraType cameraType READ getCameraType WRITE setCameraType NOTIFY cameraTypeChanged)
	Q_PROPERTY(int lightCount READ getLightCount WRITE setLightCount NOTIFY lightCountChanged)
	Q_PROPERTY(int lightPosRange READ getLightPosRange WRITE setLightPosRange NOTIFY lightPosRangeChanged)

	enum class CameraType { ORBIT, FREELOOK };
	Q_ENUM(CameraType)

	RendererQQuickItem();

	RendererQQuickItem(const RendererQQuickItem&) = delete;

	RendererQQuickItem(RendererQQuickItem&&) = default;

	RendererQQuickItem& operator=(const RendererQQuickItem&) = delete;

	RendererQQuickItem& operator=(RendererQQuickItem&&) = default;

	virtual ~RendererQQuickItem();

	void setFovy(int value);

	void setMovementSpeed(int value);

	void setLightCount(int value);

	void setLightPosRange(int value);

	void setCameraType(CameraType type);

	int getFovy() const;

	int getLightCount() const;

	int getLightPosRange() const;

	int getMovementSpeed() const;

	CameraType getCameraType() const;

signals:

	void loadScene(const QUrl& sceneFile);

	void mouseLeftPressed(const QPointF& position);

	void mouseRightPressed(const QPointF& position);

	void mouseLeftReleased(const QPointF& position);

	void mouseRightReleased(const QPointF& position);

	void mousePositionChanged(const QPointF& position);

	void keyPressed(Qt::Key key);

	void renderTimeChanged(float time);

	void sceneLoadingChanged(bool value);

	void fovyChanged(int value);

	void lightCountChanged(int value);

	void lightPosRangeChanged(int value);

	void movementSpeedChanged(int value);

	void cameraTypeChanged(CameraType type);

public slots:

	void onSynchronize();

	void onSceneGraphInvalidated();

	void onRenderingFinished(float time);

	void onSceneLoaded(std::shared_ptr<ge::sg::Scene> scene);

	void onSceneLoadingFailed();

private slots:

	void onWindowChanged(QQuickWindow* win);

	void onLoadScene(const QUrl& sceneFile);

private:

	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<QThread> m_sceneLoaderThread;
	std::unique_ptr<SceneLoader> m_sceneLoader;
	float m_renderTime;
	bool m_sceneLoading = false;
};


