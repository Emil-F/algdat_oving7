
#include <ctime>
#include <iostream>

#include "algorithm.h"
#include "init.h"

#include <QApplication>

#include <QVariant>

#include "geomap.h"

std::string formatTid(int tid) {
    int totalSekunder = tid / 100;
    int timer = totalSekunder / 3600;
    int minutter = (totalSekunder / 60) % 60;
    int sekunder = totalSekunder % 60;

    return std::format("Tid (bil) (tt:mm:ss): {:02}:{:02}:{:02}\n", timer, minutter, sekunder);
}

int main(int argc, char *argv[]) {
    /**
     *  Fire ladestasjoner nær Odense:2763740
     *  Fire spise- eller drikkestedene nærmest Gløshaugen:2001238
     *
     *  Ruter:
     *  Gløshaugen:2001238 -> Otilienborg:1987066
     *  Gløshaugen:2001238 -> Odense:2763740
     */

    QApplication app(argc, argv);

    std::unordered_map<int, Init::POI> pointsOfInterest;
    Init::initInterestPoints(pointsOfInterest, "data/norden/interessepkt.txt");

    Init::Graph graph;
    Init::initGraph(graph, "data/norden/noder.txt", "data/norden/kanter.txt");

    std::vector landmarkIds {
        545911,  // København
        2948202, // Oslo
        7826348, // Trondheim
        3370846, // Mo i Rana
        7161953, // Stockholm
        5542364  // Bergen
    };

    std::vector<std::vector<int>> landmarkDistances;

    // Init::processLandmarkDistances(graph, landmarkDistances,  landmarkIds);
    Init::readLandmarkDistances(graph, landmarkDistances,  landmarkIds);

    std::string sourceName = "Odense";
    std::string goalName = "Otilienborg";
    int source = 2763740;
    int goal = 1987066;

    std::cout << "<==> Source: [" << sourceName << "] -> Goal: [" << goalName << "] | (Dijkstra's)" << '\n';

    clock_t before = clock();

    std::vector<int> parentDijkstra;
    auto dist = Algorithm::Dijkstra(graph, source, goal, &parentDijkstra);
    clock_t duration = clock() - before;

    if (dist.at(goal) == INT_MAX) {
        std::cout << "No route from " << source << " to " << goal << '\n';
        return 0;
    }

    std::cout << "Duration (Dijkstra's): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    std::cout << formatTid(dist[goal]);
    std::cout << '\n';

    std::cout << "<==> Source: [" << sourceName << "] -> Goal: [" << goalName << "] | (ALT)" << '\n';

    before = clock();
    std::vector<int> parentAlt;
    std::vector<int> distAlt = Algorithm::ALT(graph, source, goal, landmarkDistances, &parentAlt);
    duration = clock() - before;

    std::cout << "Kjøretid (ALT): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    std::cout << formatTid(dist[goal]);
    std::cout << '\n';

    std::cout << "<==> Source: [" << sourceName << "] | (Finn nærmeste interesspunkter)" << '\n';

    before = clock();
    std::vector<int> interestPointsFound = Algorithm::findNearestInterestPoints(graph, source, pointsOfInterest, 4, 4);
    duration = clock() - before;

    std::cout << "Kjøretid (Finn nærmeste interesspunkter): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    // Your existing graph and route calculations go here.
    QQuickWidget window;
    window.setWindowTitle("Rutekart");
    window.setResizeMode(QQuickWidget::SizeRootObjectToView);
    window.resize(1000, 700);

    window.setSource(QUrl("qrc:/MapView.qml"));  // Load exactly once

    // Her kan jeg oppdatere vinduet.
    // GeoMap::showGeoMapWithRoute(window, graph, distAlt.at(goal), goal, sourceName, goalName, parentAlt);
    GeoMap::showGeoMapWithPOI(window, graph, source, sourceName, interestPointsFound, pointsOfInterest);

    window.show();

    return app.exec();
}

