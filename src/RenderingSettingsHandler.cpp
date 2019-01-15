#include <RenderingSettingsHandler.h>
#include <Renderer.h>
#include <ForwardShadingVT.h>
#include <DeferredShadingVT.h>
#include <TiledDeferredShadingVT.h>
#include <Application.h>

#include <geGL/geGL.h>
#include <geCore/Text.h>

#include <QDebug>
#include <QUrl>

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

void ts::RenderingSettingsHandler::setTileSize(int value)
{
	if (value != getTileSize())
	{
		m_tileSize = value;
		if (m_tiledDeferredVT != nullptr)
		{
			m_tiledDeferredVT->setTileSize(m_tileSize);
		}

		emit tileSizeChanged(value);
	}
}

void ts::RenderingSettingsHandler::setMaxLightsPerTile(int value)
{
	if (value != getMaxLightsPerTile())
	{
		m_maxLightsPerTile = value;
		if (m_tiledDeferredVT != nullptr)
		{
			m_tiledDeferredVT->setMaxLightsPerTile(m_maxLightsPerTile);
		}

		emit maxLightsPerTileChanged(value);
	}
}

ts::RenderingSettingsHandler::VisualizationTechnique ts::RenderingSettingsHandler::getVisualizationTechnique() const
{
	return m_currentVT;
}

int ts::RenderingSettingsHandler::getTileSize() const
{
	return m_tileSize;
}

int ts::RenderingSettingsHandler::getMaxLightsPerTile() const
{
	return m_maxLightsPerTile;
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
		std::string vsSource = Application::loadResourceFile(":/shaders/forwardVS.glsl");
		std::string fsSource = Application::loadResourceFile(":/shaders/forwardFS.glsl");

		std::unique_ptr<ForwardShadingVT> forwardVT = std::make_unique<ForwardShadingVT>();
		forwardVT->setShaders(vsSource, fsSource);
		m_renderer->setVisualizationTechnique(std::move(forwardVT));

		m_tiledDeferredVT = nullptr;
	}
	break;
	case VisualizationTechnique::DEFERRED_SHADING:
	{
		std::string geometryVsSource = Application::loadResourceFile(":/shaders/deferredGeometryVS.glsl");
		std::string geometryFsSource = Application::loadResourceFile(":/shaders/deferredGeometryFS.glsl");
		std::string lightingVsSource = Application::loadResourceFile(":/shaders/deferredLightingVS.glsl");
		std::string lightingFsSource = Application::loadResourceFile(":/shaders/deferredLightingFS.glsl");

		std::unique_ptr<DeferredShadingVT> deferredVT = std::make_unique<DeferredShadingVT>();
		deferredVT->setShaders(geometryVsSource, geometryFsSource, lightingVsSource, lightingFsSource);
		m_renderer->setVisualizationTechnique(std::move(deferredVT));

		m_tiledDeferredVT = nullptr;
	}
	break;
	case VisualizationTechnique::TILED_DEFERRED_SHADING:
	{
		std::string geometryVsSource = Application::loadResourceFile(":/shaders/deferredGeometryVS.glsl");
		std::string geometryFsSource = Application::loadResourceFile(":/shaders/deferredGeometryFS.glsl");
		std::string lightingCsSource = Application::loadResourceFile(":/shaders/tiledDeferredLightingCS.glsl");
		std::string renderVsSource = Application::loadResourceFile(":/shaders/tiledDeferredRenderVS.glsl");
		std::string renderFsSource = Application::loadResourceFile(":/shaders/tiledDeferredRenderFS.glsl");

		std::unique_ptr<TiledDeferredShadingVT> tiledDeferredVT = std::make_unique<TiledDeferredShadingVT>();
		tiledDeferredVT->setShaders(geometryVsSource, geometryFsSource, lightingCsSource, renderVsSource, renderFsSource);
		tiledDeferredVT->setTileSize(m_tileSize);
		tiledDeferredVT->setMaxLightsPerTile(m_maxLightsPerTile);
		m_tiledDeferredVT = tiledDeferredVT.get();
		m_renderer->setVisualizationTechnique(std::move(tiledDeferredVT));
	}
	break;
	default:
		break;
	}
}
