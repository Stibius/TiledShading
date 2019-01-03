#pragma once

#include <LightedSceneVT.h>

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ge
{
	namespace gl
	{
		class Program;
		class Buffer;
		class Framebuffer;
		class Texture;
	}

	namespace sg
	{
		struct PointLight;
	}
}

namespace ts
{
	/**
	* This VT draws a ge::glsg::GLScene with lights and transformations using deferred Phong shading.
	* It supports materials with ambient, diffuse, specular and emissive components and shininess.
	* It supports ambient, diffuse, specular and emissive textures.
	* Context and shaders must be set before drawing.
	*/
	class DeferredShadingVT : public LightedSceneVT
	{
	public:

		virtual ~DeferredShadingVT() = default;

		void setViewportSize(int width, int height) override;

		virtual void setShaders(
			const std::string& geometryPassVSSource, 
			const std::string& geometryPassFSSource,
			const std::string& lightingPassVSSource,
			const std::string& lightingPassFSSource);

		void drawSetup() override;

		void draw() override;

	protected:

		int m_screenWidth;
		int m_screenHeight;

		std::unique_ptr<ge::gl::VertexArray> m_sphereVAO = nullptr;
		std::unique_ptr<ge::gl::Framebuffer> m_gBuffer = nullptr;
		std::shared_ptr<ge::gl::Texture> m_gTexPosition = nullptr;
		std::shared_ptr<ge::gl::Texture> m_gTexNormal = nullptr;
		std::shared_ptr<ge::gl::Texture> m_gTexAmbient = nullptr;
		std::shared_ptr<ge::gl::Texture> m_gTexDiffuse = nullptr;
		std::shared_ptr<ge::gl::Texture> m_gTexSpecular = nullptr;
		std::shared_ptr<ge::gl::Texture> m_gTexEmissive = nullptr;
		std::shared_ptr<ge::gl::Texture> m_gTexShininess = nullptr;

		std::unique_ptr<ge::gl::Program> m_geometryPassShaderProgram = nullptr;
		std::unique_ptr<ge::gl::Program> m_stencilPassShaderProgram = nullptr;
		std::unique_ptr<ge::gl::Program> m_lightingPassShaderProgram = nullptr;
		std::string m_geometryPassVSSource;
		std::string m_geometryPassFSSource;
		static const std::string ts::DeferredShadingVT::m_stencilPassVSSource;
		static const std::string ts::DeferredShadingVT::m_stencilPassFSSource;
		std::string m_lightingPassVSSource;
		std::string m_lightingPassFSSource;

		bool m_needToCompileShaders = false;

		virtual void createGBuffer();

		virtual void geometryPass();

		virtual void stencilPass(const ge::sg::PointLight& pointLight);

		virtual void lightingPass(const ge::sg::PointLight& pointLight);
	};
}


