#include <RenderingSettingsHandler.h>
#include <Renderer.h>
#include <ForwardShadingVT.h>
#include <DeferredShadingVT.h>
#include <TiledDeferredShadingVT.h>
#include <GBufferVT.h>
#include <FileLoader.h>

#include <geGL/geGL.h>
#include <geCore/Text.h>

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

void ts::RenderingSettingsHandler::setGBufferTexture(GBufferTexture gBufferTexture)
{
	if (gBufferTexture != m_currentGBufferTexture)
	{
		m_currentGBufferTexture = gBufferTexture;

		setRenderer();

		emit gBufferTextureChanged(gBufferTexture);
		emit render();
	}
}

void ts::RenderingSettingsHandler::setTileSize(int value)
{
	if (value != m_tileSize)
	{
		m_tileSize = value;
		if (m_tiledDeferredVT != nullptr)
		{
			m_tiledDeferredVT->setTileSize(m_tileSize);
		}

		emit tileSizeChanged(value);
		emit render();
	}
}

void ts::RenderingSettingsHandler::setMaxLightsPerTile(int value)
{
	if (value != m_maxLightsPerTile)
	{
		m_maxLightsPerTile = value;
		if (m_tiledDeferredVT != nullptr)
		{
			m_tiledDeferredVT->setMaxLightsPerTile(m_maxLightsPerTile);
		}

		emit maxLightsPerTileChanged(value);
		emit render();
	}
}

void ts::RenderingSettingsHandler::setShowTiles(bool value)
{
	if (value != m_showTiles)
	{
		m_showTiles = value;
		if (m_tiledDeferredVT != nullptr)
		{
			m_tiledDeferredVT->showTiles(value);
		}

		emit showTilesChanged(value);
		emit render();
	}
}

ts::RenderingSettingsHandler::VisualizationTechnique ts::RenderingSettingsHandler::getVisualizationTechnique() const
{
	return m_currentVT;
}

ts::RenderingSettingsHandler::GBufferTexture ts::RenderingSettingsHandler::getGBufferTexture() const
{
	return m_currentGBufferTexture;
}

int ts::RenderingSettingsHandler::getTileSize() const
{
	return m_tileSize;
}

int ts::RenderingSettingsHandler::getMaxLightsPerTile() const
{
	return m_maxLightsPerTile;
}

bool ts::RenderingSettingsHandler::getShowTiles() const
{
	return m_showTiles;
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
		std::string vsSource = FileLoader::loadFile(":/shaders/forwardVS.glsl");
		std::string fsSource = FileLoader::loadFile(":/shaders/forwardFS.glsl");

		std::unique_ptr<ForwardShadingVT> forwardVT = std::make_unique<ForwardShadingVT>();
		forwardVT->setShaders(vsSource, fsSource);
		m_renderer->setVisualizationTechnique(std::move(forwardVT));

		m_tiledDeferredVT = nullptr;
	}
	break;
	case VisualizationTechnique::DEFERRED_SHADING:
	{
		std::string geometryVsSource = FileLoader::loadFile(":/shaders/deferredGeometryVS.glsl");
		std::string geometryFsSource = FileLoader::loadFile(":/shaders/deferredGeometryFS.glsl");
		std::string lightingVsSource = FileLoader::loadFile(":/shaders/deferredLightingVS.glsl");
		std::string lightingFsSource = FileLoader::loadFile(":/shaders/deferredLightingFS.glsl");

		std::unique_ptr<DeferredShadingVT> deferredVT = std::make_unique<DeferredShadingVT>();
		deferredVT->setShaders(geometryVsSource, geometryFsSource, lightingVsSource, lightingFsSource);
		m_renderer->setVisualizationTechnique(std::move(deferredVT));

		m_tiledDeferredVT = nullptr;
	}
	break;
	case VisualizationTechnique::TILED_DEFERRED_SHADING:
	{
		std::string geometryVsSource = FileLoader::loadFile(":/shaders/deferredGeometryVS.glsl");
		std::string geometryFsSource = FileLoader::loadFile(":/shaders/deferredGeometryFS.glsl");
		std::string lightingCsSource = FileLoader::loadFile(":/shaders/tiledDeferredLightingCS.glsl");
		std::string renderVsSource = FileLoader::loadFile(":/shaders/tiledDeferredRenderVS.glsl");
		std::string renderFsSource = FileLoader::loadFile(":/shaders/tiledDeferredRenderFS.glsl");

		std::unique_ptr<TiledDeferredShadingVT> tiledDeferredVT = std::make_unique<TiledDeferredShadingVT>();
		tiledDeferredVT->setShaders(geometryVsSource, geometryFsSource, lightingCsSource, renderVsSource, renderFsSource);
		tiledDeferredVT->setTileSize(m_tileSize);
		tiledDeferredVT->setMaxLightsPerTile(m_maxLightsPerTile);
		tiledDeferredVT->showTiles(m_showTiles);
		m_tiledDeferredVT = tiledDeferredVT.get();
		m_renderer->setVisualizationTechnique(std::move(tiledDeferredVT));
	}
	break;
	default:
	case VisualizationTechnique::GBUFFER:
	{
		std::string vsSource = FileLoader::loadFile(":/shaders/deferredGeometryVS.glsl");
		std::string fsSource = FileLoader::loadFile(":/shaders/deferredGeometryFS.glsl");

		std::unique_ptr<GBufferVT> gBufferVT = std::make_unique<GBufferVT>();
		gBufferVT->setShaders(vsSource, fsSource);
		gBufferVT->setDrawBuffer(static_cast<ts::GBuffer::Buffers>(m_currentGBufferTexture));
		m_renderer->setVisualizationTechnique(std::move(gBufferVT));
	}
	break;
		break;
	}
}
