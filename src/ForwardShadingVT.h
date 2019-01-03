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
	}

	namespace sg
	{
		struct PointLight;
	}
}

namespace ts
{
	/**
	* This VT draws a ge::glsg::GLScene with lights and transformations using basic forward Phong shading.
	* It supports materials with ambient, diffuse, specular and emissive components and shininess.
	* It supports ambient, diffuse, specular and emissive textures.
	* Context and shaders must be set before drawing.
	*/
	class ForwardShadingVT : public LightedSceneVT
	{
	public:

		virtual ~ForwardShadingVT() = default;

		void setViewportSize(int width, int height) override;

		virtual void setShaders(const std::string& vsSource, const std::string& fsSource);

		void drawSetup() override;

		void draw() override;

	protected:

		std::unique_ptr<ge::gl::Buffer> m_lightsShaderStorageBuffer = nullptr;
		std::unique_ptr<ge::gl::Program> m_shaderProgram = nullptr;
		std::string m_vsSource;
		std::string m_fsSource;

		bool m_needToCompileShaders = false;
	};
}

