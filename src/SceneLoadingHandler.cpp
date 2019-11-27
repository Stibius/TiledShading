#include <SceneLoadingHandler.h>
#include <Renderer.h>
#include <Scene.h>
#include <AABB.h>
#include <AssimpModelLoader.h>
#include <assimp/postprocess.h>

#include <geSG/Scene.h>
#include <geSG/DefaultImage.h>

#include <QtImageLoader.h>
#include <QFileInfo>
#include <QUrl>
#include <QtConcurrent>

ts::SceneLoadingHandler::SceneLoadingHandler(Scene* scene, Renderer* renderer)
	: m_renderer(renderer)
	, m_scene(scene)
{

}

void ts::SceneLoadingHandler::init(Scene* scene, Renderer* renderer)
{
	m_renderer = renderer;
	m_scene = scene;
}

std::shared_ptr<ge::sg::Scene> ts::SceneLoadingHandler::loadScene(const QUrl& sceneFile) const
{
	//model loading
	QString modelFileName(sceneFile.toLocalFile());
	QFileInfo fi(modelFileName);
	std::string modelPath(qUtf8Printable(fi.canonicalPath() + "/"));
	std::shared_ptr<ge::sg::Scene> scene(AssimpModelLoader::loadScene(modelFileName.toUtf8().constData(), aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals));

	if (scene && !scene->models.empty())
	{
		loadImages(*scene, modelPath);
	}

	return scene;
}

void ts::SceneLoadingHandler::loadImages(ge::sg::Scene& scene, const std::string & imageDir) const
{
	std::shared_ptr<ge::sg::DefaultImage> defaultImage(std::make_shared<ge::sg::DefaultImage>());

	for (auto model : scene.models)
	{
		for (std::shared_ptr<ge::sg::Material> material : model->materials)
		{
			for (std::vector<std::shared_ptr<ge::sg::MaterialComponent>>::const_iterator it = material->materialComponents.begin(); it != material->materialComponents.end(); ++it)
			{
				if ((*it)->getType() == ge::sg::MaterialComponent::ComponentType::IMAGE)
				{
					ge::sg::MaterialImageComponent* img = dynamic_cast<ge::sg::MaterialImageComponent*>((*it).get());
					std::string textFile(imageDir + img->filePath);
					std::shared_ptr<QtImage> image(QtImageLoader::loadImage(textFile.c_str()));
					if (image == nullptr)
					{
						img->image = defaultImage;
					}
					else {
						img->image = image;
					}
				}
			}
		}
	}
}

void ts::SceneLoadingHandler::startLoading(const QUrl& sceneFile)
{
	if (!m_initialized)
	{
		QObject::connect(&m_sceneFutureWatcher, &QFutureWatcher<std::shared_ptr<ge::sg::Scene>>::finished, this, &ts::SceneLoadingHandler::loadingFinished);
		m_initialized = true;
	}

	m_sceneFuture = QtConcurrent::run([this, sceneFile]()
	{
		return loadScene(sceneFile);
	});

	m_sceneFutureWatcher.setFuture(m_sceneFuture);
}

void ts::SceneLoadingHandler::loadingFinished()
{
	std::shared_ptr<ge::sg::Scene> scene = m_sceneFuture.result();
	if (!scene || scene->models.empty())
	{
		emit sceneLoadingFailed();
	}
	else
	{
		m_scene->setScene(*scene);
		m_renderer->setScene(*scene);
		
		emit sceneLoaded();
	}
}
