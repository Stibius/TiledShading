#include <RenderingSettingsHandler.h>
#include <Renderer.h>
#include <PhongVT.h>

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

		std::unique_ptr<PhongVT> phongVT = std::make_unique<PhongVT>();
		phongVT->setShaders(vsSource, fsSource);
		m_renderer->setVisualizationTechnique(std::move(phongVT));
	}
	break;
	case VisualizationTechnique::DEFERRED_SHADING:
	{
		std::string shaderDir(APP_RESOURCES"/shaders/");
		std::string vsSource = ge::core::loadTextFile(shaderDir + "deferredVS.glsl");
		std::string fsSource = ge::core::loadTextFile(shaderDir + "deferredFS.glsl");

		std::unique_ptr<PhongVT> phongVT = std::make_unique<PhongVT>();
		phongVT->setShaders(vsSource, fsSource);
		m_renderer->setVisualizationTechnique(std::move(phongVT));
	}
	break;
	default:
		break;
	}
}
