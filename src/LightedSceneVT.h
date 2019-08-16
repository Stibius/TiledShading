#pragma once

#include <SceneVT.h>

namespace ge
{
	namespace sg
	{
		struct PointLight;
	}
}

namespace ts
{
	/**
	* Base class for VTs which can draw a ge::glsg::GLScene with lights and transformations. 
	* This base class only implements setting and processing of the scene and saving the transformation and light values. 
	* Context must be set before setting the scene.
	*/
	class LightedSceneVT : public SceneVT
	{

	public:

		void setLights(std::shared_ptr<std::vector<ge::sg::PointLight>> pointLights);

	protected:

		std::shared_ptr<std::vector<ge::sg::PointLight>> m_pointLights = nullptr;
		
		bool m_needToSetupLights = true;
	};
}