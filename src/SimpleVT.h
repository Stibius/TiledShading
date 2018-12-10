#pragma once

#include <VisualizationTechnique.h>
#include <geSG/AttributeDescriptor.h>
#include <geGL/Generated/OpenGLTypes.h>
#include <geSG/Material.h>

#include <memory>
#include <unordered_map>

namespace ge
{
	namespace gl
	{
		class Texture;
		class Context;
		class VertexArray;
		class Program;
	}
	namespace sg 
	{
		class Scene;
		class Light;
	}
	namespace glsg
	{
		class GLScene;
	}
}

namespace ts
{
	/**
	 * This VT is used to draw a textured object.
	 * The usage of the VT is to supply the GLScene, then call the processScene. Once this is done
	 * you can call drawSetup and draw each frame. You need to have OpenGL context <b>active</b>.
	 */
	class SimpleVT : public VisualizationTechnique
	{
	public:

		SimpleVT() = default;

		virtual ~SimpleVT() = default;

		virtual void setScene(std::shared_ptr<ge::glsg::GLScene> scene);

		virtual void processScene();

		void drawSetup() override;

		void draw() override;

		static int semantic2Attribute(ge::sg::AttributeDescriptor::Semantic semantic);

		std::shared_ptr<ge::gl::Context> m_glContext = nullptr;

		std::shared_ptr<ge::gl::Program> m_shaderProgram = nullptr;

	protected:

		std::shared_ptr<ge::glsg::GLScene> m_glScene = nullptr;

		std::unordered_map<ge::sg::Mesh*, std::shared_ptr<ge::gl::VertexArray>> VAOContainer;

		std::unordered_map<ge::sg::Mesh*, std::unordered_map<ge::sg::MaterialSimpleComponent::Semantic, std::unique_ptr<unsigned char[]>>> colorContainer;

		std::unordered_map<ge::sg::Mesh*, std::unordered_map<ge::sg::MaterialImageComponent::Semantic, std::shared_ptr<ge::gl::Texture>>> textureContainer;

	};
}