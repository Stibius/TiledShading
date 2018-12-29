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
	* Shaders must be set before setting lights and transformations.
	* Context, shaders and scene must be set before drawing.
	*/
	class PhongVT : public LightedSceneVT
	{
	public:

		virtual ~PhongVT() = default;

		virtual void setShaders(const std::string& vsSource, const std::string& fsSource);

		virtual void setLights(const std::vector<ge::sg::PointLight>& pointLights);

		virtual void setProjectionMatrix(glm::mat4 projectionMatrix);

		virtual void setViewMatrix(glm::mat4 viewMatrix);

		virtual void drawSetup();

		virtual void draw();

	protected:

		std::unique_ptr<ge::gl::Buffer> m_lightsBuffer = nullptr;
		std::shared_ptr<ge::gl::Program> m_shaderProgram = nullptr;
		std::string m_vsSource;
		std::string m_fsSource;

		bool m_needToCompileShaders = false;

		virtual void compileShaders();
	};
}

