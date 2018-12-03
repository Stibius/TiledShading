#pragma once

#include <memory>

#include <QObject>

namespace ge
{
	namespace sg
	{
		class Scene;
	}
}

class SceneLoader : public QObject
{
	Q_OBJECT

public:

	SceneLoader() = default;

	virtual ~SceneLoader() = default;

public slots:

	void loadScene(const QUrl& sceneFile);

signals:

	void sceneLoaded(std::shared_ptr<ge::sg::Scene> scene);

	void sceneLoadingFailed();

private:

	/**
	* Simple function that loads images that are found at image components of materials
	* and populate its image property.
	*
	* \warning Assumes relative texture file paths or empty imageDir. There should be an extended check
	*          in production code.
	*/
	void loadImages(ge::sg::Scene& scene, const std::string& imageDir);

};
