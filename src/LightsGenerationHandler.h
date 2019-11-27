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

		LightsGenerationHandler(const LightsGenerationHandler& other) = default;

		LightsGenerationHandler(LightsGenerationHandler&& other) = default;

		LightsGenerationHandler& operator=(const LightsGenerationHandler& other) = default;

		LightsGenerationHandler& operator=(LightsGenerationHandler&& other) = default;

		~LightsGenerationHandler() override = default;

		void init(const Scene* scene, Renderer* renderer);

	signals:

		void render();

	public slots:

		void generate(int count, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float radiusMin, float radiusMax);

	protected:

		Renderer* m_renderer = nullptr;
		const Scene* m_scene = nullptr;

	};
}
