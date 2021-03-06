#pragma once

#include <LightedSceneVT.h>
#include <GBuffer.h>

#include <memory>
#include <vector>

namespace ge
{
	namespace gl
	{
		class Program;
	}

	namespace sg
	{
		struct PointLight;
	}
}

namespace ts
{
	class GBuffer;
	class GBufferVT;

	/**
	* This VT draws a ge::glsg::GLScene with lights and transformations using deferred Phong shading.
	* It supports materials with ambient, diffuse, specular and emissive components and shininess.
	* It supports ambient, diffuse, specular and emissive textures.
	* Context and shaders must be set before drawing.
	*/
	class DeferredShadingVT : public LightedSceneVT
	{
	public:

		DeferredShadingVT() = default;

		DeferredShadingVT(const DeferredShadingVT& other) = delete;

		DeferredShadingVT(DeferredShadingVT&& other) = default;

		DeferredShadingVT& operator=(const DeferredShadingVT& other) = delete;

		DeferredShadingVT& operator=(DeferredShadingVT&& other) = default;

		~DeferredShadingVT() override = default;

		void setViewportSize(int width, int height) override;

		void setShaders(
			const std::string& geometryPassVSSource, 
			const std::string& geometryPassFSSource,
			const std::string& lightingPassVSSource,
			const std::string& lightingPassFSSource);

		void drawSetup() override;

		void draw() override;

	protected:

		int m_screenWidth;
		int m_screenHeight;

		std::unique_ptr<ge::gl::VertexArray> m_sphereVAO;

		std::unique_ptr<GBuffer> m_gBuffer;

		std::unique_ptr<ge::gl::Program> m_geometryPassShaderProgram;
		std::unique_ptr<ge::gl::Program> m_stencilPassShaderProgram;
		std::unique_ptr<ge::gl::Program> m_lightingPassShaderProgram;
		std::string m_geometryPassVSSource;
		std::string m_geometryPassFSSource;
		static const std::string ts::DeferredShadingVT::m_stencilPassVSSource;
		static const std::string ts::DeferredShadingVT::m_stencilPassFSSource;
		std::string m_lightingPassVSSource;
		std::string m_lightingPassFSSource;

		bool m_needToCompileShaders = false;

		void geometryPass();

		void stencilPass(const ge::sg::PointLight& pointLight);

		void lightingPass(const ge::sg::PointLight& pointLight);
	};
}


