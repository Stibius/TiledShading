
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

		Renderer(const Renderer& other) = delete;

		Renderer(Renderer&& other) = default;

		Renderer& operator=(const Renderer& other) = delete;

		Renderer& operator=(Renderer&& other) = default;

		virtual ~Renderer() = default;

		void setViewportSize(int width, int height);

		void setVisualizationTechnique(std::unique_ptr<SceneVT> visualizationTechnique);

		void setScene(const ge::sg::Scene& scene);

		void setLights(std::shared_ptr<std::vector<ge::sg::PointLight>> pointLights);

		void setCamera(std::shared_ptr<Camera> camera);

		int render();

	protected:

		int m_viewPortWidth;
		int m_viewPortHeight;
		std::shared_ptr<ge::gl::Context> m_glContext;
		std::shared_ptr<ge::sg::Scene> m_scene;
		std::shared_ptr<ge::glsg::GLScene> m_glScene;
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<std::vector<ge::sg::PointLight>> m_pointLights;
		std::unique_ptr<ge::gl::Buffer> m_lightsShaderStorageBuffer;
		std::unique_ptr<SceneVT> m_VT;
		std::unique_ptr<ForwardShadingVT> m_noLightVT;
		bool m_needToProcessScene = false;
		bool m_needToSetLightUniforms = true;
		bool m_needToInitVT = true;

		void setupGLState();
	};
}
