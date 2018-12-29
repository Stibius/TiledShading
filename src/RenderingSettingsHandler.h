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

	class RenderingSettingsHandler : public QObject
	{
		Q_OBJECT

	public:

		Q_PROPERTY(VisualizationTechnique visualizationTechnique READ getVisualizationTechnique WRITE setVisualizationTechnique NOTIFY visualizationTechniqueChanged)

		enum class VisualizationTechnique 
		{ 
			FORWARD_SHADING, 
			DEFERRED_SHADING
		};
		Q_ENUM(VisualizationTechnique)

		RenderingSettingsHandler() = default;

		RenderingSettingsHandler(Renderer* renderer);

		virtual ~RenderingSettingsHandler() = default;

		void init(Renderer* renderer);

		void setVisualizationTechnique(VisualizationTechnique visualizationTechnique);

		VisualizationTechnique getVisualizationTechnique() const;

	signals:

		void visualizationTechniqueChanged(VisualizationTechnique visualizationTechnique);

		void render();

	protected:

		Renderer* m_renderer;
		VisualizationTechnique m_currentVT = VisualizationTechnique::FORWARD_SHADING;

		void setRenderer();
	};
}

