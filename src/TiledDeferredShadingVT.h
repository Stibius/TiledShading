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
	* This VT draws a ge::glsg::GLScene with lights and transformations using tiled deferred Phong shading.
	* It supports materials with ambient, diffuse, specular and emissive components and shininess.
	* It supports ambient, diffuse, specular and emissive textures.
	* Context and shaders must be set before drawing.
	*/
	class TiledDeferredShadingVT : public LightedSceneVT
	{
	public:

		TiledDeferredShadingVT() = default;

		TiledDeferredShadingVT(const TiledDeferredShadingVT& vt) = delete;

		TiledDeferredShadingVT(TiledDeferredShadingVT&& vt) = default;

		TiledDeferredShadingVT& operator=(const TiledDeferredShadingVT& vt) = delete;

		TiledDeferredShadingVT& operator=(TiledDeferredShadingVT&& vt) = default;

		virtual ~TiledDeferredShadingVT() = default;

		void setViewportSize(int width, int height) override;

		void setProjectionMatrix(glm::mat4 projectionMatrix) override;

		void setTileSize(int size);

		void setMaxLightsPerTile(int maxLightsPerTile);

		virtual void setShaders(
			const std::string& geometryPassVSSource,
			const std::string& geometryPassFSSource,
			const std::string& renderPassVSSource,
			const std::string& renderPassFSSource,
			const std::string& lightingPassCSSource);

		void drawSetup() override;

		void draw() override;

	protected:

		int m_screenWidth;
		int m_screenHeight;
		int m_tileSize = 16;
		float m_near;
		float m_far;

		std::unique_ptr<ge::gl::VertexArray> m_dummyVAO = nullptr;

		std::unique_ptr<GBuffer> m_gBuffer = nullptr;
		std::unique_ptr<ge::gl::Buffer> m_lightsShaderStorageBuffer = nullptr;
		std::unique_ptr<ge::gl::Texture> m_outputTexture = nullptr;

		std::unique_ptr<ge::gl::Program> m_geometryPassShaderProgram = nullptr;
		std::unique_ptr<ge::gl::Program> m_lightingPassShaderProgram = nullptr;
		std::unique_ptr<ge::gl::Program> m_renderPassShaderProgram = nullptr;
		std::string m_geometryPassVSSource;
		std::string m_geometryPassFSSource;
		std::string m_lightingPassCSSource;
		std::string m_renderPassVSSource;
		std::string m_renderPassFSSource;

		bool m_needToCompileShaders = false;

		virtual void geometryPass();

		virtual void lightingPass();

		virtual void renderPass();
	};
}



