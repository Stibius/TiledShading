#include <LightedSceneVT.h>

void ts::LightedSceneVT::setLights(std::shared_ptr<std::vector<ge::sg::PointLight>> pointLights)
{
	m_pointLights = pointLights;
	m_needToSetupLights = true;
}
