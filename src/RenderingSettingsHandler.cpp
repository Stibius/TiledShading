#include <RenderingSettingsHandler.h>
#include <Renderer.h>
#include <ForwardShadingVT.h>
#include <DeferredShadingVT.h>

#include <geGL/geGL.h>
#include <geCore/Text.h>

ts::RenderingSettingsHandler::RenderingSettingsHandler(Renderer* renderer)
	: m_renderer(renderer)
{
	setRenderer();
}

void ts::RenderingSettingsHandler::init(Renderer* renderer)
{
	m_renderer = renderer;

	setRenderer();
}

void ts::RenderingSettingsHandler::setVisualizationTechnique(VisualizationTechnique visualizationTechnique)
{
	if (visualizationTechnique != getVisualizationTechnique())
	{
		m_currentVT = visualizationTechnique;

		setRenderer();

		emit visualizationTechniqueChanged(visualizationTechnique);
		emit render();
	}
}

ts::RenderingSettingsHandler::VisualizationTechnique ts::RenderingSettingsHandler::getVisualizationTechnique() const
{
	return m_currentVT;
}

void ts::RenderingSettingsHandler::setRenderer()
{
	if (!m_renderer)
	{
		return;
	}

	switch (m_currentVT)
	{
	case VisualizationTechnique::FORWARD_SHADING:
	{
		std::string shaderDir(APP_RESOURCES"/shaders/");
		std::string vsSource = ge::core::loadTextFile(shaderDir + "forwardVS.glsl");
		std::string fsSource = ge::core::loadTextFile(shaderDir + "forwardFS.glsl");

		std::unique_ptr<ForwardShadingVT> forwardVT = std::make_unique<ForwardShadingVT>();
		forwardVT->setShaders(vsSource, fsSource);
		m_renderer->setVisualizationTechnique(std::move(forwardVT));
	}
	break;
	case VisualizationTechnique::DEFERRED_SHADING:
	{
		std::string shaderDir(APP_RESOURCES"/shaders/");
		std::string geometryVsSource = ge::core::loadTextFile(shaderDir + "deferredGeometryVS.glsl");
		std::string geometryFsSource = ge::core::loadTextFile(shaderDir + "deferredGeometryFS.glsl");
		std::string lightingVsSource = ge::core::loadTextFile(shaderDir + "deferredLightingVS.glsl");
		std::string lightingFsSource = ge::core::loadTextFile(shaderDir + "deferredLightingFS.glsl");

		std::unique_ptr<DeferredShadingVT> deferredVT = std::make_unique<DeferredShadingVT>();
		deferredVT->setShaders(geometryVsSource, geometryFsSource, lightingVsSource, lightingFsSource);
		m_renderer->setVisualizationTechnique(std::move(deferredVT));
	}
	break;
	default:
		break;
	}
}
