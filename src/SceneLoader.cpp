#include <SceneLoader.h>

#include <geSG/Scene.h>
#include <geSG/DefaultImage.h>
#include <AssimpModelLoader.h>
#include <assimp/postprocess.h>
#include <QtImageLoader.h>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

void ts::SceneLoader::loadScene(const QUrl& sceneFile)
{
	//model loading
	QString modelFileName(sceneFile.toLocalFile());
	QFileInfo fi(modelFileName);
	std::string modelPath(qUtf8Printable(fi.canonicalPath() + "/"));
	std::shared_ptr<ge::sg::Scene> scene(AssimpModelLoader::loadScene(modelFileName.toUtf8().constData(), aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals));

	if (!scene || scene->models.empty())
	{
		emit sceneLoadingFailed();
	}
	else
	{
		loadImages(*scene, modelPath);
		emit sceneLoaded(scene);
	}
}

void ts::SceneLoader::loadImages(ge::sg::Scene& scene, const std::string & imageDir)
{
	std::shared_ptr<ge::sg::DefaultImage> defaultImage(std::make_shared<ge::sg::DefaultImage>());

	for (auto model : scene.models)
	{
		for (std::shared_ptr<ge::sg::Material> material : model->materials)
		{
			for (std::vector<std::shared_ptr<ge::sg::MaterialComponent>>::iterator it = material->materialComponents.begin(); it != material->materialComponents.end(); ++it)
			{
				if ((*it)->getType() == ge::sg::MaterialComponent::ComponentType::IMAGE)
				{
					ge::sg::MaterialImageComponent* img = dynamic_cast<ge::sg::MaterialImageComponent*>((*it).get());
					std::string textFile(imageDir + img->filePath);
					std::shared_ptr<QtImage> image(QtImageLoader::loadImage(textFile.c_str()));
					if (image == nullptr)
					{
						//qDebug() << "  " << "FAILED TO LOAD!" << "substituting default image\n";
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
