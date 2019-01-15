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
		Q_PROPERTY(int tileSize READ getTileSize WRITE setTileSize NOTIFY tileSizeChanged)
		Q_PROPERTY(int maxLightsPerTile READ getMaxLightsPerTile WRITE setMaxLightsPerTile NOTIFY maxLightsPerTileChanged)

		enum class VisualizationTechnique 
		{ 
			FORWARD_SHADING, 
			DEFERRED_SHADING,
			TILED_DEFERRED_SHADING
		};
		Q_ENUM(VisualizationTechnique)

		RenderingSettingsHandler() = default;

		RenderingSettingsHandler(Renderer* renderer);

		virtual ~RenderingSettingsHandler() = default;

		void init(Renderer* renderer);

		void setVisualizationTechnique(VisualizationTechnique visualizationTechnique);

		void setTileSize(int value);

		void setMaxLightsPerTile(int value);

		VisualizationTechnique getVisualizationTechnique() const;

		int getTileSize() const;

		int getMaxLightsPerTile() const;

	signals:

		void visualizationTechniqueChanged(VisualizationTechnique visualizationTechnique);

		void tileSizeChanged(int value);

		void maxLightsPerTileChanged(int value);

		void render();

	protected:

		Renderer* m_renderer = nullptr;
		TiledDeferredShadingVT* m_tiledDeferredVT = nullptr;
		VisualizationTechnique m_currentVT = VisualizationTechnique::FORWARD_SHADING;
		int m_tileSize = 16;
		int m_maxLightsPerTile = 256;

		void setRenderer();
	};
}

