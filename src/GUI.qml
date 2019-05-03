
import QtQuick 2.0
import TiledShading 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.2

ApplicationWindow
{
    id: mainWindow

	width: 800
    height: 600

	title: "Tiled Shading"
    visible: true
	color: "black"

	Connections 
	{
        target: sceneLoadingHandler 
        onSceneLoaded:
		{
			loadSceneAction.enabled = true;

			loadingModelText.text = qsTr("Scene loaded successfully");

			sbLightCount.value = 0;
			sbLightCount.oldValue = 0;
			lightsGenerationHandler.generate(0, 0, 0, 0, 0, 0, 0, 0, 0);
        }
    }

	Connections 
	{
        target: sceneLoadingHandler 
        onSceneLoadingFailed:
		{
		    loadSceneAction.enabled = true;

			loadingModelText.text = qsTr("Scene loading failed");
        }
    }

	FileDialog 
    {  
        id: loadSceneDialog

        title: "Load scene"
        folder: shortcuts.home

        onAccepted: 
	    {
			loadSceneAction.enabled = false;
			loadingModelText.text = qsTr("Loading scene...")
			sceneLoadingHandler.startLoading(loadSceneDialog.fileUrl)
        }
    }

	Action 
	{
        id: loadSceneAction

        text: "Load scene"

        onTriggered: 
		{
            loadSceneDialog.visible = true;
        }
    }

	Action 
	{
        id: settingsAction

        text: "Settings"

        onTriggered: 
		{
            settingsWindow.visible = true;
        }
    }

	menuBar: MenuBar 
	{
	    id: menu

        Menu 
		{
            title: "Menu"

            MenuItem { action: loadSceneAction }
			MenuItem { action: settingsAction }
			MenuItem { text: "Quit"; onTriggered: Qt.quit() }
        }
    }
    
    RendererItem 
	{  
	    id: rendererItem
		objectName: "rendererItem"

		anchors.fill: parent
		focus: true
    }

	footer: ToolBar 
	{
	    height: 25
	
        RowLayout 
		{
		    anchors.leftMargin: 5
			anchors.rightMargin: 5
            anchors.fill: parent

            Label 
			{ 
			    id: renderTimeText

			    text: qsTr("Render time: ") + rendererItem.renderTime.toFixed(2) + qsTr(" ms")
			}

			Label 
			{ 
			    id: loadingModelText

				Layout.alignment: Qt.AlignRight

			    text: qsTr("")
			}
        }
    }

	Window
    {
        id: settingsWindow

		width: 500
        height: 500

		title: "Settings"
        visible: false

		onClosing: 
		{
			sbLightCount.value = sbLightCount.oldValue;
			sbLightXPosMin.value = sbLightXPosMin.oldValue;
			sbLightYPosMin.value = sbLightYPosMin.oldValue;
			sbLightZPosMin.value = sbLightZPosMin.oldValue;
			sbLightXPosMax.value = sbLightXPosMax.oldValue;
			sbLightYPosMax.value = sbLightYPosMax.oldValue;
			sbLightZPosMax.value = sbLightZPosMax.oldValue;
			sbPointLightRadiusMin.value = sbPointLightRadiusMin.oldValue;
			sbPointLightRadiusMax.value = sbPointLightRadiusMax.oldValue;
		}

        TabBar 
		{
            id: bar
            width: parent.width

            TabButton 
			{
                text: qsTr("Camera")
            }
            TabButton 
			{
                text: qsTr("Lights")
            }
            TabButton 
			{
                text: qsTr("Rendering")
            }
        }
		
		StackLayout 
		{
            width: parent.width
            currentIndex: bar.currentIndex
			anchors.top: parent.top;
			anchors.left: parent.left;
			anchors.topMargin: bar.height * 2
			anchors.leftMargin: 10

			GridLayout 
			{
            	columns: 2
				rows: 5
            	rowSpacing: 10
            	columnSpacing: 10
 
				Label 
				{
					Layout.column: 0
                	Layout.row: 0

                	text: "Camera type:"
            	}
				RowLayout
				{
                	spacing: 10

					ButtonGroup 
			    	{ 
			        	id: cameraTypeGroup 
				    	exclusive: true
			    	}
					RadioButton 
			    	{
                    	Layout.column: 0
				    	ButtonGroup.group: cameraTypeGroup

                    	text: "Orbit"
                    	checked: cameraSettingsHandler.cameraType == CameraSettingsHandler.ORBIT
                
			       	 	onClicked: cameraSettingsHandler.cameraType = CameraSettingsHandler.ORBIT;
                	}
                	RadioButton 
			    	{
			        	Layout.column: 1
				    	ButtonGroup.group: cameraTypeGroup

                    	text: "Freelook"
				    	checked: cameraSettingsHandler.cameraType == CameraSettingsHandler.FREELOOK

			       		onClicked: cameraSettingsHandler.cameraType = CameraSettingsHandler.FREELOOK;
                	}
				}

				Label 
				{
					Layout.column: 0
                	Layout.row: 1

                	text: "Field of view:"
            	}
				SpinBox 
				{
                	id: sbFovy

					Layout.column: 1
                	Layout.row: 1
				
                	value: cameraSettingsHandler.fovy
                	from: 0
					to: 180 
					editable: true

					textFromValue: function(value, locale) 
					{
                    	return Number(value).toLocaleString(locale, 'f', 0);
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text);
                	}

					Binding { target: sbFovy; property: "value"; value: cameraSettingsHandler.fovy }
                	Binding { target: cameraSettingsHandler; property: "fovy"; value: sbFovy.value }
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 2

               		text: "Step size:"
					visible: cameraSettingsHandler.cameraType == CameraSettingsHandler.FREELOOK
            	}
				SpinBox 
				{
                	id: sbStepSize

					Layout.column: 1
                	Layout.row: 2

                	value: cameraSettingsHandler.relativeStepSize * 1000
                	from: 0
					to: 1000
					stepSize: 100
					editable: true
					visible: cameraSettingsHandler.cameraType == CameraSettingsHandler.FREELOOK

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbStepSize.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
				
					Binding { target: sbStepSize; property: "value"; value: cameraSettingsHandler.relativeStepSize * 1000 }
               		Binding { target: cameraSettingsHandler; property: "relativeStepSize"; value: sbStepSize.value / 1000 }
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 3

                	text: "Zooming speed:"
					visible: cameraSettingsHandler.cameraType == CameraSettingsHandler.ORBIT
            	}
				SpinBox 
				{
                	id: sbZoomSpeed

					Layout.column: 1
                	Layout.row: 3

                	value: cameraSettingsHandler.relativeZoomSpeed * 1000
                	from: 0
					to: 1000
					stepSize: 100
					editable: true
					visible: cameraSettingsHandler.cameraType == CameraSettingsHandler.ORBIT

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbZoomSpeed.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
				
					Binding { target: sbZoomSpeed; property: "value"; value: cameraSettingsHandler.relativeZoomSpeed * 1000 }
                	Binding { target: cameraSettingsHandler; property: "relativeZoomSpeed"; value: sbZoomSpeed.value / 1000 }
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 4

                	text: "Rotation speed:"
            	}
				SpinBox 
				{
                	id: sbRotationSpeed

					Layout.column: 1
                	Layout.row: 4

                	value: cameraSettingsHandler.rotationSpeed * 1000
                	from: 0
					to: 1000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbRotationSpeed.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
				
					Binding { target: sbRotationSpeed; property: "value"; value: cameraSettingsHandler.rotationSpeed * 1000 }
                	Binding { target: cameraSettingsHandler; property: "rotationSpeed"; value: sbRotationSpeed.value / 1000 }
            	}
			}

			GridLayout 
			{
            	columns: 4
				rows: 6
            	rowSpacing: 10
            	columnSpacing: 10

				Label 
				{
					Layout.column: 0
                	Layout.row: 0

                	text: "Light count:"
            	}
				SpinBox 
				{
                	id: sbLightCount

					Layout.column: 1
                	Layout.row: 0

					property int oldValue: 0;

                	value: 0
                	from: 0
					to: 10000
					editable: true
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 1

                	text: "Light x-position range:"
            	}
				SpinBox 
				{
                	id: sbLightXPosMin

					Layout.column: 1
               		Layout.row: 1

					property int oldValue: -500;

					value: -500
                	from: -100000
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbLightXPosMin.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}
				Label 
				{
					Layout.column: 2
                	Layout.row: 1

                	text: "-"
            	}
				SpinBox 
				{
                	id: sbLightXPosMax

					Layout.column: 3
               		Layout.row: 1

					property int oldValue: 500;

					value: 500
                	from: -100000
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbLightXPosMax.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 2

                	text: "Light y-position range:"
            	}
				SpinBox 
				{
                	id: sbLightYPosMin

					Layout.column: 1
               		Layout.row: 2

					property int oldValue: -500;

					value: -500
                	from: -100000
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbLightYPosMin.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}
				Label 
				{
					Layout.column: 2
                	Layout.row: 2

                	text: "-"
            	}
				SpinBox 
				{
                	id: sbLightYPosMax

					Layout.column: 3
               		Layout.row: 2

					property int oldValue: 500;

					value: 500
                	from: -100000
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbLightYPosMax.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 3

                	text: "Light z-position range:"
            	}
				SpinBox 
				{
                	id: sbLightZPosMin

					Layout.column: 1
               		Layout.row: 3

					property int oldValue: -500;

					value: -500
                	from: -100000
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbLightZPosMin.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}
				Label 
				{
					Layout.column: 2
                	Layout.row: 3

                	text: "-"
            	}
				SpinBox 
				{
                	id: sbLightZPosMax

					Layout.column: 3
               		Layout.row: 3

					property int oldValue: 500;

					value: 500
                	from: -100000
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbLightZPosMax.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 4

                	text: "Point light radius:"
            	}
				SpinBox 
				{
                	id: sbPointLightRadiusMin

					Layout.column: 1
                	Layout.row: 4

					property int oldValue: 100;

					value: 100
                	from: 0
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbPointLightRadiusMin.decimals)
                	}

               		valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}
				Label 
				{
					Layout.column: 2
                	Layout.row: 4

                	text: "-"
            	}
				SpinBox 
				{
                	id: sbPointLightRadiusMax

					Layout.column: 3
                	Layout.row: 4

					property int oldValue: 500;

					value: 500
                	from: 0
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbPointLightRadiusMax.decimals)
                	}

                	valueFromText: function(text, locale) 
					{
                    	return Number.fromLocaleString(locale, text) * 1000
                	}
            	}

				Button 
				{
			    	Layout.column: 1
                	Layout.row: 5

                	text: "Generate lights"

					onClicked: 
					{
						if (sbPointLightRadiusMin.value > sbPointLightRadiusMax.value)
						{
					    	sbPointLightRadiusMax.value = sbPointLightRadiusMin.value;
						}
						if (sbLightXPosMin.value > sbLightXPosMax.value)
						{
					    	sbLightXPosMax.value = sbLightXPosMin.value;
						}
						if (sbLightYPosMin.value > sbLightYPosMax.value)
						{
					    	sbLightYPosMax.value = sbLightYPosMin.value;
						}
						if (sbLightZPosMin.value > sbLightZPosMax.value)
						{
					    	sbLightZPosMax.value = sbLightZPosMin.value;
						}

						sbLightCount.oldValue = sbLightCount.value;
						sbLightXPosMin.oldValue = sbLightXPosMin.value;
						sbLightYPosMin.oldValue = sbLightYPosMin.value;
						sbLightZPosMin.oldValue = sbLightZPosMin.value;
						sbLightXPosMax.oldValue = sbLightXPosMax.value;
						sbLightYPosMax.oldValue = sbLightYPosMax.value;
						sbLightZPosMax.oldValue = sbLightZPosMax.value;
						sbPointLightRadiusMin.oldValue = sbPointLightRadiusMin.value;
						sbPointLightRadiusMax.oldValue = sbPointLightRadiusMax.value;

						var count = sbLightCount.value;
						var minX = sbLightXPosMin.value / 1000;
						var maxX = sbLightXPosMax.value / 1000;
						var minY = sbLightYPosMin.value / 1000;
						var maxY = sbLightYPosMax.value / 1000;
						var minZ = sbLightZPosMin.value / 1000;
						var maxZ = sbLightZPosMax.value / 1000;
						var radiusMin = sbPointLightRadiusMin.value / 1000;
						var radiusMax = sbPointLightRadiusMax.value / 1000;

						lightsGenerationHandler.generate(count, minX, maxX, minY, maxY, minZ, maxZ, radiusMin, radiusMax);
					}
            	}
        	}

			GridLayout 
			{
            	columns: 2
				rows: 6
            	rowSpacing: 10
            	columnSpacing: 10

				Label 
				{
					Layout.column: 0
                	Layout.row: 0

                	text: "Rendering area size:"
            	}
				Label 
				{
					Layout.column: 1
                	Layout.row: 0

                	text: rendererItem.width + " x " + rendererItem.height
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 1

                	text: "Rendering technique:"
            	}
				ComboBox 
				{
				    id: cbVisualizationTechnique

				    Layout.column: 1
                	Layout.row: 1
					Layout.preferredWidth: 180

                    model: ["Forward Shading", "Deferred Shading", "Tiled Deferred Shading", "GBuffer"]

					currentIndex: renderingSettingsHandler.visualizationTechnique

					Binding { target: cbVisualizationTechnique; property: "currentIndex"; value: renderingSettingsHandler.visualizationTechnique }
                	Binding { target: renderingSettingsHandler; property: "visualizationTechnique"; value: cbVisualizationTechnique.currentIndex }
                }

				Label 
				{
					Layout.column: 0
                	Layout.row: 2

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.GBUFFER

                	text: "GBuffer texture:"
            	}
				ComboBox 
				{
				    id: cbGBufferTexture

				    Layout.column: 1
                	Layout.row: 2
					Layout.preferredWidth: 180

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.GBUFFER

                    model: ["Position", "Normal", "Ambient", "Diffuse", "Specular", "Emissive", "Shininess"]

					currentIndex: renderingSettingsHandler.gBufferTexture

					Binding { target: cbGBufferTexture; property: "currentIndex"; value: renderingSettingsHandler.gBufferTexture }
                	Binding { target: renderingSettingsHandler; property: "gBufferTexture"; value: cbGBufferTexture.currentIndex }
                }

				Label 
				{
					Layout.column: 0
                	Layout.row: 3

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.TILED_DEFERRED_SHADING

                	text: "Tile size:"
            	}
				SpinBox 
				{
                	id: sbTileSize

					Layout.column: 1
                	Layout.row: 3

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.TILED_DEFERRED_SHADING

					value: renderingSettingsHandler.tileSize
                	from: 1
					to: 128
					stepSize: 1
					editable: true

					Binding { target: sbTileSize; property: "value"; value: renderingSettingsHandler.tileSize }
                	Binding { target: renderingSettingsHandler; property: "tileSize"; value: sbTileSize.value }
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 4

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.TILED_DEFERRED_SHADING

                	text: "Max. lights per tile:"
            	}
				SpinBox 
				{
                	id: sbMaxLightsPerTile

					Layout.column: 1
                	Layout.row: 4

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.TILED_DEFERRED_SHADING

					value: renderingSettingsHandler.maxLightsPerTile
                	from: 1
					to: 1000
					stepSize: 1
					editable: true

					Binding { target: sbMaxLightsPerTile; property: "value"; value: renderingSettingsHandler.maxLightsPerTile }
                	Binding { target: renderingSettingsHandler; property: "maxLightsPerTile"; value: sbMaxLightsPerTile.value }
            	}

				Label 
				{
					Layout.column: 0
                	Layout.row: 5

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.TILED_DEFERRED_SHADING

                	text: "Show tiles:"
            	}
				CheckBox 
				{
                	id: chkShowTiles

					Layout.column: 1
                	Layout.row: 5

					visible: renderingSettingsHandler.visualizationTechnique == RenderingSettingsHandler.TILED_DEFERRED_SHADING

					checked: renderingSettingsHandler.showTiles

					Binding { target: chkShowTiles; property: "checked"; value: renderingSettingsHandler.showTiles }
                	Binding { target: renderingSettingsHandler; property: "showTiles"; value: chkShowTiles.checked }
            	}
			}
		}
    }
}
