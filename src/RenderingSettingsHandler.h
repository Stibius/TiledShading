#pragma once

#include <memory>

#include <QObject>

namespace ge
{
	namespace gl
	{
		class Program;
		class Context;
	}
}

namespace ts
{
	class Renderer;
	class TiledDeferredShadingVT;

	class RenderingSettingsHandler : public QObject
	{
		Q_OBJECT

	public:

		Q_PROPERTY(VisualizationTechnique visualizationTechnique READ getVisualizationTechnique WRITE setVisualizationTechnique NOTIFY visualizationTechniqueChanged)
		Q_PROPERTY(GBufferTexture gBufferTexture READ getGBufferTexture WRITE setGBufferTexture NOTIFY gBufferTextureChanged)
		Q_PROPERTY(int tileSize READ getTileSize WRITE setTileSize NOTIFY tileSizeChanged)
		Q_PROPERTY(int maxLightsPerTile READ getMaxLightsPerTile WRITE setMaxLightsPerTile NOTIFY maxLightsPerTileChanged)
		Q_PROPERTY(bool showTiles READ getShowTiles WRITE setShowTiles NOTIFY showTilesChanged)

		enum class VisualizationTechnique 
		{ 
			FORWARD_SHADING, 
			DEFERRED_SHADING,
			TILED_DEFERRED_SHADING,
			GBUFFER
		};
		Q_ENUM(VisualizationTechnique)

		enum class GBufferTexture
		{
			POSITION_TEXTURE,
			NORMAL_TEXTURE,
			AMBIENT_TEXTURE,
			DIFFUSE_TEXTURE,
			SPECULAR_TEXTURE,
			EMISSIVE_TEXTURE,
			SHININESS_TEXTURE
		};
		Q_ENUM(GBufferTexture)

		RenderingSettingsHandler() = default;

		RenderingSettingsHandler(Renderer* renderer);

		virtual ~RenderingSettingsHandler() = default;

		void init(Renderer* renderer);

		void setVisualizationTechnique(VisualizationTechnique visualizationTechnique);

		void setGBufferTexture(GBufferTexture gBufferTexture);

		void setTileSize(int value);

		void setMaxLightsPerTile(int value);

		void setShowTiles(bool value);

		VisualizationTechnique getVisualizationTechnique() const;

		GBufferTexture getGBufferTexture() const;

		int getTileSize() const;

		int getMaxLightsPerTile() const;

		bool getShowTiles() const;

	signals:

		void visualizationTechniqueChanged(VisualizationTechnique visualizationTechnique);

		void gBufferTextureChanged(GBufferTexture gBufferTexture);

		void tileSizeChanged(int value);

		void maxLightsPerTileChanged(int value);

		void showTilesChanged(bool value);

		void render();

	protected:

		Renderer* m_renderer = nullptr;
		TiledDeferredShadingVT* m_tiledDeferredVT = nullptr;
		VisualizationTechnique m_currentVT = VisualizationTechnique::FORWARD_SHADING;
		GBufferTexture m_currentGBufferTexture = GBufferTexture::POSITION_TEXTURE;
		int m_tileSize = 16;
		int m_maxLightsPerTile = 256;
		bool m_showTiles = false;

		void setRenderer();
	};
}

