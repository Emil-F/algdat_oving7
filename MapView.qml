import QtQuick
import QtLocation
import QtPositioning

Map {
    id: map
    required property var routePath
    required property string startName
    required property string goalName

    plugin: Plugin { name: "osm" }
    center: QtPositioning.coordinate(64.1466, -21.9426)
    zoomLevel: 13

    MapPolyline {
        id: routeLine
        line.width: 5
        line.color: "royalblue"
        path: map.routePath
    }

    onMapReadyChanged: {
        if (mapReady && routePath.length > 1)
            fitViewportToMapItems([routeLine])
    }

    MapItemView {
        model: map.routePath.length > 0 ? [
            { position: map.routePath[0],                        title: map.startName },
            { position: map.routePath[map.routePath.length - 1], title: map.goalName }
        ] : []

        delegate: MapQuickItem {
            required property var modelData

            coordinate: modelData.position
            anchorPoint.x: label.width / 2
            anchorPoint.y: label.height
            z: 2

            sourceItem: Rectangle {
                id: label
                width: caption.implicitWidth + 12
                height: caption.implicitHeight + 6
                color: "white"
                border.color: "royalblue"
                radius: 4

                Text {
                    id: caption
                    anchors.centerIn: parent
                    text: modelData.title
                    color: "black"
                }
            }
        }
    }
}