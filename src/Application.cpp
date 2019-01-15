#include <Application.h>
#include <Renderer.h>
#include <RendererQQuickItem.h>
#include <Camera.h>
#include <Scene.h>
#include <AABB.h>
#include <LightsGenerationHandler.h>
#include <MouseEventHandler.h>
#include <KeyEventHandler.h>
#include <CameraSettingsHandler.h>
#include <RenderingSettingsHandler.h>
#include <SceneLoadingHandler.h>

#include <QSurfaceFormat>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <geSG/Scene.h>

bool Application::m_initialized = false;
QGuiApplication* Application::m_qGuiApplication = nullptr;
ts::LightsGenerationHandler Application::m_lightsGenerationHandler;
ts::SceneLoadingHandler Application::m_sceneLoadingHandler;
ts::MouseEventHandler Application::m_mouseEventHandler;
ts::KeyEventHandler Application::m_keyEventHandler;
ts::CameraSettingsHandler Application::m_cameraSettingsHandler;
ts::RenderingSettingsHandler Application::m_renderingSettingsHandler;
std::shared_ptr<ts::Camera> Application::m_camera = std::make_shared<ts::Camera>();
ts::Scene Application::m_scene;

std::string Application::loadResourceFile(const std::string& path)
{
	QString content;
	QFile inFile(QString::fromStdString(path));
	if (inFile.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream inStream(&inFile);
		content = inStream.readAll();
		inFile.close();
	}

	return content.toStdString();
}

bool Application::init(int& argc, char* argv[], int& exitCode)
{
	// protect against multiple initializations
	exitCode = 0;
	if (m_initialized)
	{
		return true;
	}
	m_initialized = true;

	m_qGuiApplication = new QGuiApplication(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	qmlRegisterType<ts::RendererQQuickItem>("TiledShading", 1, 0, "RendererItem");
	qmlRegisterUncreatableType<ts::CameraSettingsHandler>("TiledShading", 1, 0, "CameraSettingsHandler", "Error: only enums");
	qmlRegisterUncreatableType<ts::RenderingSettingsHandler>("TiledShading", 1, 0, "RenderingSettingsHandler", "Error: only enums");

	return true;
}

int Application::run()
{
	if (!m_initialized)
	{
		return -1;
	}

    m_cameraSettingsHandler.init(m_camera.get(), &m_scene);

	QQmlApplicationEngine qmlEngine;
	qmlEngine.rootContext()->setContextProperty("lightsGenerationHandler", &m_lightsGenerationHandler);
	qmlEngine.rootContext()->setContextProperty("sceneLoadingHandler", &m_sceneLoadingHandler);
	qmlEngine.rootContext()->setContextProperty("cameraSettingsHandler", &m_cameraSettingsHandler);
	qmlEngine.rootContext()->setContextProperty("renderingSettingsHandler", &m_renderingSettingsHandler);
	QQmlComponent component(&qmlEngine, "qrc:/GUI.qml");
	QObject* object = component.create();

	ts::RendererQQuickItem* rendererItem = object->findChild<ts::RendererQQuickItem*>("rendererItem");
	rendererItem->installEventFilter(&m_mouseEventHandler);
	rendererItem->installEventFilter(&m_keyEventHandler);

	ts::Renderer* renderer = rendererItem->getRenderer();
	renderer->setCamera(m_camera);

	m_mouseEventHandler.init(m_camera.get());
	m_keyEventHandler.init(m_camera.get());
	m_lightsGenerationHandler.init(&m_scene, renderer);
	m_sceneLoadingHandler.init(&m_scene, renderer);
	m_renderingSettingsHandler.init(renderer);

	QObject::connect(&m_renderingSettingsHandler, &ts::RenderingSettingsHandler::render, rendererItem, &ts::RendererQQuickItem::render);
	QObject::connect(&m_cameraSettingsHandler, &ts::CameraSettingsHandler::render, rendererItem, &ts::RendererQQuickItem::render);
	QObject::connect(&m_keyEventHandler, &ts::KeyEventHandler::render, rendererItem, &ts::RendererQQuickItem::render);
	QObject::connect(&m_mouseEventHandler, &ts::MouseEventHandler::render, rendererItem, &ts::RendererQQuickItem::render);
	QObject::connect(&m_lightsGenerationHandler, &ts::LightsGenerationHandler::render, rendererItem, &ts::RendererQQuickItem::render);
	QObject::connect(&m_sceneLoadingHandler, &ts::SceneLoadingHandler::sceneLoaded, rendererItem, &ts::RendererQQuickItem::render);
	QObject::connect(&m_sceneLoadingHandler, &ts::SceneLoadingHandler::sceneLoaded, &m_cameraSettingsHandler, &ts::CameraSettingsHandler::sceneChanged);
	QObject::connect(&m_cameraSettingsHandler, &ts::CameraSettingsHandler::absoluteStepSizeChanged, &m_keyEventHandler, &ts::KeyEventHandler::absoluteStepSizeChanged);
	QObject::connect(&m_cameraSettingsHandler, &ts::CameraSettingsHandler::absoluteZoomSpeedChanged, &m_mouseEventHandler, &ts::MouseEventHandler::absoluteZoomSpeedChanged);
	QObject::connect(&m_cameraSettingsHandler, &ts::CameraSettingsHandler::rotationSpeedChanged, &m_mouseEventHandler, &ts::MouseEventHandler::rotationSpeedChanged);
	
	return m_qGuiApplication->exec();
}

void Application::cleanUp()
{
	if (!m_initialized)
	{
		return;
	}
	m_initialized = false;

	delete m_qGuiApplication;
	m_qGuiApplication = nullptr;
}
