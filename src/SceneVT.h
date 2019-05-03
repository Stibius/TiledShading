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
	}

	namespace glsg
	{
		class GLScene;
	}
}

namespace ts
{
	/**
	* Base class for VTs which can draw a ge::glsg::GLScene transformations.
	* This base class only implements setting and processing of the scene and saving the transformation values.
	* Context must be set before setting the scene.
	*/
	class SceneVT : public VisualizationTechnique
	{

	public:

		std::shared_ptr<ge::gl::Context> m_glContext = nullptr;

		SceneVT() = default;

		SceneVT(const SceneVT& vt) = delete;

		SceneVT(SceneVT&& vt) = default;

		SceneVT& operator=(const SceneVT& vt) = delete;

		SceneVT& operator=(SceneVT&& vt) = default;

		virtual ~SceneVT() = default;

		virtual void setViewportSize(int width, int height) = 0;

		virtual void setScene(std::shared_ptr<ge::glsg::GLScene> scene);

		virtual int semantic2Attribute(ge::sg::AttributeDescriptor::Semantic semantic);

		virtual void setProjectionMatrix(glm::mat4 projectionMatrix);

		virtual void setViewMatrix(glm::mat4 viewMatrix);

		virtual void setModelMatrix(glm::mat4 modelMatrix);

	protected:

		std::shared_ptr<ge::glsg::GLScene> m_glScene = nullptr;
		std::unordered_map<ge::sg::Mesh*, std::shared_ptr<ge::gl::VertexArray>> VAOContainer;
		std::unordered_map<ge::sg::Mesh*, std::unordered_map<ge::sg::MaterialSimpleComponent::Semantic, std::unique_ptr<unsigned char[]>>> colorContainer;
		std::unordered_map<ge::sg::Mesh*, std::unordered_map<ge::sg::MaterialImageComponent::Semantic, std::shared_ptr<ge::gl::Texture>>> textureContainer;

		glm::mat4 m_projectionMatrix = glm::mat4(1.0);
		glm::mat4 m_viewMatrix = glm::mat4(1.0);
		glm::mat4 m_modelMatrix = glm::mat4(1.0);

		bool m_sceneProcessed = false;
		bool m_needToSetupUniforms = true;

		virtual void processScene();
	};
}
