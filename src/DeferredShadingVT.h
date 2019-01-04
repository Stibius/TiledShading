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

		DeferredShadingVT(const DeferredShadingVT& vt) = delete;

		DeferredShadingVT(DeferredShadingVT&& vt) = default;

		DeferredShadingVT& operator=(const DeferredShadingVT& vt) = delete;

		DeferredShadingVT& operator=(DeferredShadingVT&& vt) = default;

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

		std::unique_ptr<GBuffer> m_gBuffer = nullptr;

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

		virtual void geometryPass();

		virtual void stencilPass(const ge::sg::PointLight& pointLight);

		virtual void lightingPass(const ge::sg::PointLight& pointLight);
	};
}


