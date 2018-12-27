#pragma once

#include <VisualizationTechnique.h>

#include <memory>
#include <vector>
#include <unordered_map>

#include <geSG/AttributeDescriptor.h>
#include <geSG/Material.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ge
{
	namespace gl
	{
		class Context;
		class VertexArray;
		class Texture;
	}

	namespace sg
	{
		class Mesh;
		struct PointLight;
	}

	namespace glsg
	{
		class GLScene;
	}
}

namespace ts
{
	/**
	* Base class for VTs which can draw a ge::glsg::GLScene with lights and transformations. 
	* This base class only implements setting and processing of the scene. 
	* Context must be set before setting the scene.
	*/
	class LightedSceneVT : public VisualizationTechnique
	{

	public:

		std::shared_ptr<ge::gl::Context> m_glContext = nullptr;

		virtual ~LightedSceneVT() = default;

		virtual void setScene(std::shared_ptr<ge::glsg::GLScene> scene);

		virtual int semantic2Attribute(ge::sg::AttributeDescriptor::Semantic semantic);

		virtual void setLights(const std::vector<ge::sg::PointLight>& pointLights) = 0;

		virtual void setProjectionMatrix(glm::mat4 projectionMatrix) = 0;

		virtual void setViewMatrix(glm::mat4 viewMatrix) = 0;

	protected:

		std::shared_ptr<ge::glsg::GLScene> m_glScene = nullptr;
		std::unordered_map<ge::sg::Mesh*, std::shared_ptr<ge::gl::VertexArray>> VAOContainer;
		std::unordered_map<ge::sg::Mesh*, std::unordered_map<ge::sg::MaterialSimpleComponent::Semantic, std::unique_ptr<unsigned char[]>>> colorContainer;
		std::unordered_map<ge::sg::Mesh*, std::unordered_map<ge::sg::MaterialImageComponent::Semantic, std::shared_ptr<ge::gl::Texture>>> textureContainer;

		bool m_sceneProcessed = false;

		virtual void processScene();
	};
}