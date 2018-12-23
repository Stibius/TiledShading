
#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ge 
{
	namespace gl 
	{
		class Program;
		class Context;
		class Buffer;
	}

	namespace sg
	{
		class Scene;
		struct PointLight;
	}
}

namespace ts
{
	class SimpleVT;
	class Camera;

	class Renderer 
	{

	public:

		Renderer();

		Renderer(const Renderer&) = delete;

		Renderer(Renderer&&) = default;

		Renderer& operator=(const Renderer&) = delete;

		Renderer& operator=(Renderer&&) = default;

		virtual ~Renderer() = default;

		void setViewportSize(int width, int height);

		void loadShaders(const std::string& vsPath, const std::string& fsPath);

		virtual void setScene(const ge::sg::Scene& scene);

		virtual void setLights(const std::vector<ge::sg::PointLight>& pointLights);

		virtual void setCamera(std::shared_ptr<Camera> camera);

		virtual int render();

	protected:

		int m_viewPortWidth;
		int m_viewPortHeight;
		std::shared_ptr<ge::gl::Program> m_shaderProgram = nullptr;
		std::shared_ptr<ge::gl::Context> m_glContext = nullptr;
		std::shared_ptr<ge::sg::Scene> m_scene = nullptr;
		std::shared_ptr<Camera> m_camera = nullptr;
		std::vector<ge::sg::PointLight> m_pointLights;
		std::unique_ptr<ge::gl::Buffer> m_lightsBuffer = nullptr;
		std::unique_ptr<SimpleVT> m_visualizationTechnique = nullptr;
		bool m_needToProcessScene = false;
		bool m_needToSetLightUniforms = true;

		virtual void setupGLState();
	};
}
