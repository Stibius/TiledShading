#pragma once

#include <QtQuick/QQuickItem>

#include <memory>

namespace ts
{
	class Renderer;

	class RendererQQuickItem : public QQuickItem
	{
		Q_OBJECT

	public:

		Q_PROPERTY(float renderTime MEMBER m_renderTime NOTIFY renderTimeChanged)
		Q_PROPERTY(bool showRenderTime READ getShowRenderTime WRITE setShowRenderTime NOTIFY showRenderTimeChanged)

		RendererQQuickItem();

		RendererQQuickItem(const RendererQQuickItem&) = delete;

		RendererQQuickItem(RendererQQuickItem&&) = default;

		RendererQQuickItem& operator=(const RendererQQuickItem&) = delete;

		RendererQQuickItem& operator=(RendererQQuickItem&&) = default;

		virtual ~RendererQQuickItem() = default;

		void setShowRenderTime(bool value);

		Renderer* getRenderer() const;

		bool getShowRenderTime() const;

	signals:

		void renderTimeChanged(float time);

		void showRenderTimeChanged(bool value);

	public slots:

		virtual void onSynchronize();

		virtual void render();

	private slots:

		virtual void onWindowChanged(QQuickWindow* win);

		virtual void beforeRendering();

	protected:

		std::unique_ptr<Renderer> m_renderer = std::make_unique<Renderer>();
		float m_renderTime;
		bool m_showRenderTime = true;
		bool m_firstDraw = true;
	};
}


