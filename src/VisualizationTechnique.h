#pragma once

namespace ts
{
	/**
	 * Base class for Visualization techniques (VT). VTs are used to visualize something, mainly
	 * the scene graph or some part of scene in some user pre defined way. VT could stand for visualizing
	 * the ge::sg::Scene (or some processed variation such as ge::glsg::GLScene) via phong shading. It could
	 * be use to render skybox without even knowing the scene etc.
	 * The VTs are the corner stones of visualization. The rendering part of frame should be a sequence of
	 * VT::draw() calls.
	 */
	class VisualizationTechnique
	{

	public:

		VisualizationTechnique() = default;

		VisualizationTechnique(const VisualizationTechnique& other) = delete;

		VisualizationTechnique(VisualizationTechnique&& other) = default;

		VisualizationTechnique& operator=(const VisualizationTechnique& other) = delete;

		VisualizationTechnique& operator=(VisualizationTechnique&& other) = default;

		virtual ~VisualizationTechnique() = default;

		virtual void drawSetup() = 0;

		virtual void draw() = 0;

	};
}
