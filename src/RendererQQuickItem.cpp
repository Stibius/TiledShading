#include <RendererQQuickItem.h>
#include <Renderer.h>

#include <QtQuick/qquickwindow.h>

ts::RendererQQuickItem::RendererQQuickItem()
{
	setAcceptedMouseButtons(Qt::AllButtons);

	connect(this, &QQuickItem::windowChanged, this, &RendererQQuickItem::onWindowChanged);
}

void ts::RendererQQuickItem::render()
{
	window()->update();
}

ts::Renderer* ts::RendererQQuickItem::getRenderer() const
{
	return m_renderer.get();
}

void ts::RendererQQuickItem::onWindowChanged(QQuickWindow* win)
{
	if (win != nullptr)
	{
		connect(win, &QQuickWindow::beforeSynchronizing, this, &RendererQQuickItem::onSynchronize, Qt::DirectConnection);
		connect(win, &QQuickWindow::beforeRendering, this, &RendererQQuickItem::beforeRendering, Qt::DirectConnection);

		// If we allow QML to do the clearing, they would clear what we paint
		// and nothing would show.
		win->setClearBeforeRendering(false);
	}
}

void ts::RendererQQuickItem::beforeRendering()
{
	float time = m_renderer->render() / 1000.0f;
	window()->resetOpenGLState();

	static bool firstDraw = true; // updating rendering time in the GUI causes another redraw, only do it once
	if (firstDraw)
	{
		m_renderTime = time;
		emit renderTimeChanged(time);
	}

	firstDraw = !firstDraw;
}

void ts::RendererQQuickItem::onSynchronize()
{
	m_renderer->setViewportSize(
		window()->size().width() * window()->devicePixelRatio(), 
		window()->size().height() * window()->devicePixelRatio());
}
