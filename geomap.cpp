//
// Created by Emil-PC on 19.11.2026.
//

#include "geomap.h"
#include <QQuickWidget>

#include <QGeoCoordinate>
#include <QVariant>
#include <QQuickItem>

#include "init.h"

void GeoMap::showGeoMapWithRoute(
    QQuickWidget& window,
    const Init::Graph& graph,
    int goalDistance,
    int goal,
    const std::string& sourceName,
    const std::string& goalName,
    const std::vector<int>& parent
) {
    auto* map = window.rootObject();
    if (!map) {
        qWarning() << "Load MapView.qml before updating the map";
        return;
    }

    QVariantList routePath;

    if (goalDistance != INT_MAX) {
        for (int node = goal; node != -1; node = parent[node]) {
            const auto& pos = graph.coordinates.at(node);
            routePath.append(QVariant::fromValue(
                QGeoCoordinate(pos.breddegrad, pos.lengdegrad)
            ));
        }

        std::reverse(routePath.begin(), routePath.end());
    }

    map->setProperty("routePath", routePath);
    map->setProperty("startName", QString::fromStdString(sourceName));
    map->setProperty("goalName", QString::fromStdString(goalName));
    map->setProperty("poiMarkers", QVariantList{});
}

void GeoMap::showGeoMapWithPOI(
    QQuickWidget& window,
    const Init::Graph &graph,
    int source,
    const std::string& sourceName,
    const std::vector<int>& foundPOI,
    const std::unordered_map<int, Init::POI>& POI
) {
    auto* map = window.rootObject();
    if (!map) {
        qWarning() << "Load MapView.qml before updating the map";
        return;
    }

    QVariantList poiMarkers;

    for (int node : foundPOI) {
        const auto& pos = graph.coordinates.at(node);

        QVariantMap marker;
        marker["name"] = QString::fromStdString(POI.at(node).navn);
        marker["position"] = QVariant::fromValue(
            QGeoCoordinate(pos.breddegrad, pos.lengdegrad)
        );

        poiMarkers.append(marker);
    }

    map->setProperty("routePath", QVariantList{});
    map->setProperty("startName", QString::fromStdString(sourceName));
    map->setProperty("goalName", QString{});
    map->setProperty("poiMarkers", poiMarkers);

    const auto& pos = graph.coordinates.at(source);
    map->setProperty("sourceCoordinate", QVariant::fromValue(
        QGeoCoordinate(pos.breddegrad, pos.lengdegrad)
    ));
}
