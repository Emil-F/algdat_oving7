import QtQuick
import QtLocation
import QtPositioning

Map {
    id: map
    property var routePath: []
    property string startName: ""
    property string goalName: ""

    plugin: Plugin { name: "osm" }

    property var sourceCoordinate: null

    function fitMap() {
        if (!mapReady) return

        if (routePath.length >= 2) {
            fitViewportToMapItems([routeLine])
            return
        }

        if (!sourceCoordinate || !sourceCoordinate.isValid) return

        let radius = 0
        for (const marker of poiMarkers) {
            const distance = sourceCoordinate.distanceTo(marker.position)
            radius = Math.max(radius, distance)
        }

        if (radius > 0) {
            const area = QtPositioning.circle(sourceCoordinate, radius)
            fitViewportToGeoShape(area, 40)
        }

        center = sourceCoordinate
    }

    onMapReadyChanged: Qt.callLater(map.fitMap)
    onSourceCoordinateChanged: Qt.callLater(map.fitMap)
    onPoiMarkersChanged: Qt.callLater(map.fitMap)

    MapPolyline {
        id: routeLine
        path: map.routePath
        line.width: 5
        line.color: "royalblue"

        onPathChanged: Qt.callLater(map.fitMap)
    }

    MapItemView {
        model: {
            if (map.routePath.length > 0) {
                return [
                    { position: map.routePath[0], title: map.startName },
                    { position: map.routePath[map.routePath.length - 1], title: map.goalName }
                ]
            }

            if (map.sourceCoordinate) {
                return [{ position: map.sourceCoordinate, title: map.startName }]
            }

            return []
        }

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

    property var poiMarkers: []

    MapItemView {
        model: map.poiMarkers

        delegate: MapQuickItem {
            id: marker
            required property var modelData

            coordinate: modelData.position
            anchorPoint.x: 6
            anchorPoint.y: 6
            z: 3

            sourceItem: Row {
                spacing: 4

                Rectangle {
                    width: 12
                    height: 12
                    radius: 6
                    color: "lightblue"
                    border.color: "black"
                }

                Text {
                    text: marker.modelData.name
                    color: "black"
                    style: Text.Outline
                    styleColor: "white"
                }
            }
        }
    }
}