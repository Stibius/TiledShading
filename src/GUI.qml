
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
			sbLightPosRange.value = sbLightPosRange.oldValue;
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
				rows: 4
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

                	text: "Light position range:"
            	}
				SpinBox 
				{
                	id: sbLightPosRange

					Layout.column: 1
               		Layout.row: 1

					property int oldValue: 2000;

					value: 2000
                	from: 0
					to: 100000
					stepSize: 100
					editable: true

					property int decimals: 3

					textFromValue: function(value, locale) 
					{
                    	return Number(value / 1000).toLocaleString(locale, 'f', sbLightPosRange.decimals)
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

                	text: "Point light radius:"
            	}
				SpinBox 
				{
                	id: sbPointLightRadiusMin

					Layout.column: 1
                	Layout.row: 2

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
                	Layout.row: 2

                	text: "-"
            	}
				SpinBox 
				{
                	id: sbPointLightRadiusMax

					Layout.column: 3
                	Layout.row: 2

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
                	Layout.row: 3

                	text: "Generate lights"

					onClicked: 
					{
						if (sbPointLightRadiusMin.value > sbPointLightRadiusMax.value)
						{
					    	sbPointLightRadiusMax.value = sbPointLightRadiusMin.value;
						}

						sbLightCount.oldValue = sbLightCount.value;
						sbLightPosRange.oldValue = sbLightPosRange.value;
						sbPointLightRadiusMin.oldValue = sbPointLightRadiusMin.value;
						sbPointLightRadiusMax.oldValue = sbPointLightRadiusMax.value;

						var count = sbLightCount.value;
						var posRange = sbLightPosRange.value / 1000;
						var radiusMin = sbPointLightRadiusMin.value / 1000;
						var radiusMax = sbPointLightRadiusMax.value / 1000;

						lightsGenerationHandler.generate(count, posRange, radiusMin, radiusMax);
					}
            	}
        	}

			GridLayout 
			{
            	columns: 2
				rows: 1
            	rowSpacing: 10
            	columnSpacing: 10

				Label 
				{
					Layout.column: 0
                	Layout.row: 0

                	text: "Rendering technique:"
            	}
				ComboBox 
				{
				    id: cbVisualizationTechnique

				    Layout.column: 1
                	Layout.row: 0

                    model: ["Forward Shading", "Deferred Shading"]

					currentIndex: renderingSettingsHandler.visualizationTechnique

					Binding { target: cbVisualizationTechnique; property: "currentIndex"; value: renderingSettingsHandler.visualizationTechnique }
                	Binding { target: renderingSettingsHandler; property: "visualizationTechnique"; value: cbVisualizationTechnique.currentIndex }
                }
			}
		}
    }
}
