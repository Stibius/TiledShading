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

		RendererQQuickItem();

		RendererQQuickItem(const RendererQQuickItem&) = delete;

		RendererQQuickItem(RendererQQuickItem&&) = default;

		RendererQQuickItem& operator=(const RendererQQuickItem&) = delete;

		RendererQQuickItem& operator=(RendererQQuickItem&&) = default;

		virtual ~RendererQQuickItem() = default;

		Renderer* getRenderer() const;

	signals:

		void renderTimeChanged(float time);

	public slots:

		virtual void onSynchronize();

		virtual void render();

	private slots:

		virtual void onWindowChanged(QQuickWindow* win);

		virtual void beforeRendering();

	protected:

		std::unique_ptr<Renderer> m_renderer = std::make_unique<Renderer>();
		float m_renderTime;
	};
}


