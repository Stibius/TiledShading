
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

	namespace glsg
	{
		class GLScene;
	}
}

namespace ts
{
	class Camera;
	class SceneVT;
	class ForwardShadingVT;

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

		virtual void setVisualizationTechnique(std::unique_ptr<SceneVT> visualizationTechnique);

		virtual void setScene(const ge::sg::Scene& scene);

		virtual void setLights(std::shared_ptr<std::vector<ge::sg::PointLight>> pointLights);

		virtual void setCamera(std::shared_ptr<Camera> camera);

		virtual int render();

	protected:

		int m_viewPortWidth;
		int m_viewPortHeight;
		std::shared_ptr<ge::gl::Context> m_glContext = nullptr;
		std::shared_ptr<ge::sg::Scene> m_scene = nullptr;
		std::shared_ptr<ge::glsg::GLScene> m_glScene = nullptr;
		std::shared_ptr<Camera> m_camera = nullptr;
		std::shared_ptr<std::vector<ge::sg::PointLight>> m_pointLights = nullptr;
		std::unique_ptr<ge::gl::Buffer> m_lightsShaderStorageBuffer = nullptr;
		std::unique_ptr<SceneVT> m_VT = nullptr;
		std::unique_ptr<ForwardShadingVT> m_noLightVT = nullptr;
		bool m_needToProcessScene = false;
		bool m_needToSetLightUniforms = true;
		bool m_needToInitVT = true;

		virtual void setupGLState();
	};
}
