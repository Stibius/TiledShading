
#pragma once

#include <QtQuick/QQuickItem>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ge 
{
	namespace gl 
	{
		class Program;
		class Context;
		class Buffer;
	}

	namespace sg
	{
		class Scene;
		class SceneAABB;
		class Light;
	}

	namespace glsg 
	{
		class GLScene;
	}

	namespace util 
	{
		class CameraTransform;
		class PerspectiveCamera;
	}
}

class SimpleVT;

class Renderer : public QObject
{
	Q_OBJECT

public:

	enum class CameraType { ORBIT, FREELOOK };

	Renderer();

	Renderer(const Renderer&) = delete;

	Renderer(Renderer&&) = default;

	Renderer& operator=(const Renderer&) = delete;

	Renderer& operator=(Renderer&&) = default;

	virtual ~Renderer() = default;

	void setViewportSize(const QSize& size);

	void setWindow(QQuickWindow* window);

	void setScene(std::shared_ptr<ge::sg::Scene> scene);

	void setupGLState();

	void loadShaders(const std::string& vsPath, const std::string& fsPath);

	void setFovy(float value);

	void setMovementSpeed(float value);

	void setLightPosRange(float value);

	void generateLights(int count);

	void setCameraType(CameraType type);

	float getFovy() const;

	int getLightCount() const;

	float getLightPosRange() const;

	float getMovementSpeed() const;

	CameraType getCameraType() const;

signals:

	void redraw();

	void renderingFinished(float time);

public slots:

	void onRender();

	void onMouseLeftPressed(const QPointF& position);

	void onMouseRightPressed(const QPointF& position);

	void onMouseLeftReleased(const QPointF& position);

	void onMouseRightReleased(const QPointF& position);

	void onMousePositionChanged(const QPointF& position);

	void onKeyPressed(Qt::Key key);

private:

	QSize m_viewportSize;
	std::shared_ptr<ge::gl::Program> m_shaderProgram = nullptr;
	std::shared_ptr<ge::gl::Context> m_glContext = nullptr;
	std::shared_ptr<ge::sg::Scene> m_scene = nullptr;
	std::unique_ptr<ge::util::CameraTransform> m_transformCamera = nullptr;
	std::unique_ptr<ge::util::PerspectiveCamera> m_perspectiveCamera = nullptr;
	std::vector<ge::sg::Light> m_lights;
	std::unique_ptr<ge::gl::Buffer> m_lightsBuffer = nullptr;
	std::unique_ptr<SimpleVT> m_VT = nullptr;
	std::unique_ptr<ge::sg::SceneAABB> m_boundingBox = nullptr;
	QQuickWindow* m_window = nullptr;
	float m_fovy = 45.0f;
	float m_rotationSpeedCoef = 0.5f;
	float m_movementSpeedCoef = 0.1f;
	float m_near = 0.1f;
	float m_far = 1000.0f;
	float m_lightPosRange = 10.0f;
	bool m_needToProcessScene = false;
	bool m_needToSetLightUniforms = true;
	bool m_mouseRightPressed = false;
	bool m_mouseLeftPressed = false;
	glm::vec2 m_lastLeftMousePos;
	glm::vec2 m_lastRightMousePos;
	CameraType m_cameraType = CameraType::ORBIT;

	void resetCamera();
};
