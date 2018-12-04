
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

			    text: qsTr("Loading model...")
				visible: rendererItem.sceneLoading
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
            rendererItem.loadScene(loadSceneDialog.fileUrl);
        }
    }

	Action 
	{
        id: loadSceneAction

        text: "Load scene"
		enabled: !rendererItem.sceneLoading

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

		anchors.fill: parent
		focus: true

		MouseArea 
		{  
            anchors.fill: parent;

			acceptedButtons: Qt.LeftButton | Qt.RightButton

			onPressed: 
			{
			    if (mouse.button == Qt.LeftButton)
			        rendererItem.mouseLeftPressed(Qt.point(mouse.x, mouse.y));
			    else if (mouse.button == Qt.RightButton)
				    rendererItem.mouseRightPressed(Qt.point(mouse.x, mouse.y));
			}

            onReleased: 
			{
			    if (mouse.button == Qt.LeftButton)
			        rendererItem.mouseLeftReleased(Qt.point(mouse.x, mouse.y));
			    else if (mouse.button == Qt.RightButton)
				    rendererItem.mouseRightReleased(Qt.point(mouse.x, mouse.y));
			}

            onPositionChanged: rendererItem.mousePositionChanged(Qt.point(mouse.x, mouse.y));
        }

		Keys.onPressed: 
		{
		    switch (event.key)
			{
				case Qt.Key_W:
				case Qt.Key_S:
				case Qt.Key_A:
				case Qt.Key_D:
				case Qt.Key_Space:
				case Qt.Key_C:
				    rendererItem.keyPressed(event.key);
				    break;
				default:
				    break;
			}

			event.accepted = true;
        }
    }

	Window
    {
        id: settingsWindow

		width: 500
        height: 400

		title: "Settings"
        visible: false

		onClosing: 
		{
		    sbLightCount.value = rendererItem.lightCount;
			sbLightPosRange.value = rendererItem.lightPosRange;
			sbPointLightRadiusMin.value = rendererItem.pointLightRadiusMin;
			sbPointLightRadiusMax.value = rendererItem.pointLightRadiusMax;
		}

		GridLayout 
		{
            columns: 4
			rows: 7
            rowSpacing: 10
            columnSpacing: 10
            anchors.top: parent.top;
			anchors.left: parent.left;
			anchors.topMargin: 10
			anchors.leftMargin: 10

			Label 
			{
				Layout.column: 0
                Layout.row: 0

                text: "Camera type:"
            }
			RowLayout
			{
			    Layout.columnSpan: 3
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
                    checked: rendererItem.cameraType == RendererItem.ORBIT
                
			        onClicked: rendererItem.cameraType = RendererItem.ORBIT;
                }
                RadioButton 
			    {
			        Layout.column: 1
				    ButtonGroup.group: cameraTypeGroup

                    text: "Freelook"
				    checked: rendererItem.cameraType == RendererItem.FREELOOK

			        onClicked: rendererItem.cameraType = RendererItem.FREELOOK;
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
				
                value: rendererItem.fovy
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

				onValueChanged: 
				{
				    if (value != rendererItem.fovy)
					{
					    rendererItem.fovy = value;
					}
				}
            }

			Label 
			{
				Layout.column: 0
                Layout.row: 2

                text: "Movement speed:"
            }
			SpinBox 
			{
                id: sbMovementSpeed

				Layout.column: 1
                Layout.row: 2

                value: rendererItem.movementSpeed
                from: 0
				to: 1000
				stepSize: 100
				editable: true

				property int decimals: 3

				textFromValue: function(value, locale) 
				{
                    return Number(value / 1000).toLocaleString(locale, 'f', sbMovementSpeed.decimals)
                }

                valueFromText: function(text, locale) 
				{
                    return Number.fromLocaleString(locale, text) * 1000
                }
				
				onValueChanged: 
				{
				    if (value != rendererItem.movementSpeed)
					{
					    rendererItem.movementSpeed = value;
					}
				}
            }

			Label 
			{
				Layout.column: 0
                Layout.row: 3

                text: "Light count:"
            }
			SpinBox 
			{
                id: sbLightCount

				Layout.column: 1
                Layout.row: 3

                value: rendererItem.lightCount
                from: 0
				to: 10000
				editable: true
            }

			Label 
			{
				Layout.column: 0
                Layout.row: 4

                text: "Light position range:"
            }
			SpinBox 
			{
                id: sbLightPosRange

				Layout.column: 1
                Layout.row: 4

                value: rendererItem.lightPosRange
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
                Layout.row: 5

                text: "Point light radius:"
            }
			SpinBox 
			{
                id: sbPointLightRadiusMin

				Layout.column: 1
                Layout.row: 5

                value: rendererItem.pointLightRadiusMin
                from: 0
				to: 1000
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
                Layout.row: 5

                text: "-"
            }
			SpinBox 
			{
                id: sbPointLightRadiusMax

				Layout.column: 3
                Layout.row: 5

                value: rendererItem.pointLightRadiusMax
                from: 0
				to: 1000
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
                Layout.row: 6

                text: "Generate lights"

				onClicked: 
				{
				    if (sbLightPosRange.value != rendererItem.lightPosRange)
					{
					    rendererItem.lightPosRange = sbLightPosRange.value;
					}
					if (sbPointLightRadiusMin.value != rendererItem.pointLightRadiusMin)
					{
					    rendererItem.pointLightRadiusMin = sbPointLightRadiusMin.value;
					}
					if (sbPointLightRadiusMax.value != rendererItem.pointLightRadiusMax)
					{
					    rendererItem.pointLightRadiusMax = sbPointLightRadiusMax.value;
					}

				    rendererItem.lightCount = sbLightCount.value;
				}
            }
        }
    }
}
