
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
#include <RendererQQuickItem.h>

using namespace std;

int main(int argc, char** argv)
{
	QGuiApplication app(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	qmlRegisterType<ts::RendererQQuickItem>("TiledShading", 1, 0, "RendererItem");

	QQmlApplicationEngine qmlEngine("qrc:/GUI.qml");

	return app.exec();
}