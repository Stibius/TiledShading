#pragma once

#include <QObject>

namespace ts
{
	class Renderer;
	class Scene;

	class LightsGenerationHandler : public QObject
	{
		Q_OBJECT

	public:

		LightsGenerationHandler() = default;

		LightsGenerationHandler(const Scene* scene, Renderer* renderer);

		virtual ~LightsGenerationHandler() = default;

		void init(const Scene* scene, Renderer* renderer);

	signals:

		void render();

	public slots:

		void generate(int count, float posRange, float radiusMin, float radiusMax);

	protected:

		Renderer* m_renderer = nullptr;
		const Scene* m_scene = nullptr;

	};
}
