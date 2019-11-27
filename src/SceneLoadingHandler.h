#pragma once

#include <memory>

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>

namespace ge
{
	namespace sg
	{
		class Scene;
	}
}

namespace ts
{
	class Renderer;
	class Scene;

	class SceneLoadingHandler : public QObject
	{
		Q_OBJECT

	public:

		SceneLoadingHandler() = default;

		SceneLoadingHandler(Scene* scene, Renderer* renderer);

		SceneLoadingHandler(const SceneLoadingHandler& other) = default;

		SceneLoadingHandler(SceneLoadingHandler&& other) = default;

		SceneLoadingHandler& operator=(const SceneLoadingHandler& other) = default;

		SceneLoadingHandler& operator=(SceneLoadingHandler&& other) = default;

		~SceneLoadingHandler() override = default;

		void init(Scene* scene, Renderer* renderer);

	signals:

		void sceneLoadingFailed();

		void sceneLoaded();

	public slots:

		void startLoading(const QUrl& sceneFile);

	private slots:

		void loadingFinished();

	protected:

		Renderer* m_renderer = nullptr;
		Scene* m_scene = nullptr;

		QFuture<std::shared_ptr<ge::sg::Scene>> m_sceneFuture;
		QFutureWatcher<std::shared_ptr<ge::sg::Scene>> m_sceneFutureWatcher;

		bool m_initialized = false;

		std::shared_ptr<ge::sg::Scene> loadScene(const QUrl& sceneFile) const;

		/**
		* Simple function that loads images that are found at image components of materials
		* and populate its image property.
		*
		* \warning Assumes relative texture file paths or empty imageDir. There should be an extended check
		*          in production code.
		*/
		void loadImages(ge::sg::Scene& scene, const std::string& imageDir) const;
	};
}
