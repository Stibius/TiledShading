#pragma once

#include <QGuiApplication>

namespace ts
{
	class LightsGenerationHandler;
	class SceneLoadingHandler;
	class MouseEventHandler;
	class KeyEventHandler;
	class CameraSettingsHandler;
	class RenderingSettingsHandler;
	class Camera;
	class Scene;
}

class Application
{
public:

	static bool m_initialized;
	static std::unique_ptr<QGuiApplication> m_qGuiApplication;
	static ts::LightsGenerationHandler m_lightsGenerationHandler;
	static ts::SceneLoadingHandler m_sceneLoadingHandler;
	static ts::MouseEventHandler m_mouseEventHandler;
	static ts::KeyEventHandler m_keyEventHandler;
	static ts::CameraSettingsHandler m_cameraSettingsHandler;
	static ts::RenderingSettingsHandler m_renderingSettingsHandler;
	static std::shared_ptr<ts::Camera> m_camera;
	static ts::Scene m_scene;

	static bool init(int& argc, char* argv[], int& exitCode);

	static int run();

	static void cleanUp();
};
