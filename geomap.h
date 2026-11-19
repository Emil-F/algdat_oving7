//
// Created by Emil-PC on 19.11.2026.
//

#ifndef OVING7_GEOMAP_H
#define OVING7_GEOMAP_H

#include <QQuickWidget>

#include <QGeoCoordinate>
#include "init.h"

namespace GeoMap {
    void showGeoMapWithRoute(
        QQuickWidget& window,
        const Init::Graph& graph,
        int goalDistance,
        int goal,
        const std::string& sourceName,
        const std::string& goalName,
        const std::vector<int>& parent
    );

    void showGeoMapWithPOI(
        QQuickWidget& window,
        const Init::Graph &graph,
        int source,
        const std::string& sourceName,
        const std::vector<int>& foundPOI,
        const std::unordered_map<int, Init::POI>& POI
    );
}

#endif //OVING7_GEOMAP_H
