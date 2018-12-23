#include <Scene.h>
#include <AABB.h>

#include <geSG/Scene.h>

ts::Scene::Scene(const ge::sg::Scene& scene)
{
	m_scene = std::make_unique<ge::sg::Scene>(scene);
	m_boundingBox = std::make_unique<ts::AABB>(scene);
}

ts::Scene::Scene(const Scene& scene)
{
	m_scene = std::make_unique<ge::sg::Scene>(*scene.m_scene);
	m_boundingBox = std::make_unique<ts::AABB>(*scene.m_boundingBox);
}

ts::Scene& ts::Scene::operator=(const Scene& scene)
{
	if (this == &scene)
	{
		return *this;
	}

	m_scene = std::make_unique<ge::sg::Scene>(*scene.m_scene);
	m_boundingBox = std::make_unique<ts::AABB>(*scene.m_boundingBox);

	return *this;
}

void ts::Scene::setScene(const ge::sg::Scene& scene)
{
	m_scene = std::make_unique<ge::sg::Scene>(scene);
	m_boundingBox = std::make_unique<ts::AABB>(scene);
}

const ge::sg::Scene& ts::Scene::getScene() const
{
	return *m_scene;
}

const ts::AABB& ts::Scene::getBoundingBox() const
{
	return *m_boundingBox;
}
