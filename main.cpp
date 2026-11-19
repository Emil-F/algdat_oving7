
#include <ctime>
#include <iostream>

#include "algorithm.h"
#include "init.h"

#include <QApplication>
#include <QQuickWidget>
#include <QUrl>

#include <algorithm>
#include <QGeoCoordinate>
#include <QVariant>

std::string formatTid(int tid) {
    int totalSekunder = tid / 100;
    int timer = totalSekunder / 3600;
    int minutter = (totalSekunder / 60) % 60;
    int sekunder = totalSekunder % 60;

    return std::format("Tid (bil) (tt:mm:ss): {:02}:{:02}:{:02}\n", timer, minutter, sekunder);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Stjørdal node: 2114236
    // Steinkjer node: 2044600

    // 3942 64.1303419 -21.8707187 Domino's
    // 62192 64.3215983 -22.0701350 Subway
    std::unordered_map<int, Init::POI> pointsOfInterest;
    Init::initInterestPoints(pointsOfInterest, "data/norden/interessepkt.txt");

    Init::Graph graph;
    Init::initGraph(graph, "data/norden/noder.txt", "data/norden/kanter.txt");

    std::vector landmarkIds {
        545911,
        2948202,
        7826348,
        3370846,
        7161953
    };

    std::vector<std::vector<int>> landmarkDistances;

    Init::readLandmarkDistances(graph, landmarkDistances,  landmarkIds);

    int source = 2001238;
    int goal = 2763740;

    auto sourceName = pointsOfInterest.find(source)->second.navn;
    auto goalName = pointsOfInterest.find(goal)->second.navn;

    std::cout << "<==> Source: [" << sourceName << "] -> Goal: [" << goalName << "] | (Dijkstra's)" << '\n';

    clock_t before = clock();

    std::vector<int> parentDijkstra;
    auto dist = Algorithm::Dijkstra(graph, source, goal, &parentDijkstra);
    clock_t duration = clock() - before;

    std::cout << "Duration (Dijkstra's): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    std::cout << formatTid(dist[goal]);
    std::cout << '\n';

    std::cout << "<==> Source: [" << sourceName << "] -> Goal: [" << goalName << "] | (ALT)" << '\n';

    before = clock();
    std::vector<int> parentAlt;
    dist = Algorithm::ALT(graph, source, goal, landmarkDistances, &parentAlt);
    duration = clock() - before;

    std::cout << "Duration (ALT): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    std::cout << formatTid(dist[goal]);
    std::cout << '\n';

    std::cout << "<==> Source: [" << sourceName << "] | (Finn nærmeste interesspunkter)" << '\n';

    before = clock();
    std::vector<int> interestPointsFound = Algorithm::findNearestInterestPoints(graph, source, pointsOfInterest, 16, 4);
    duration = clock() - before;

    std::cout << "Duration (Finn nærmeste interesspunkter): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    // Your existing graph and route calculations go here.

    QVariantList routePath;

    if (dist[goal] != INT_MAX) {
        for (int node = goal; node != -1; node = parentAlt[node]) {
            const auto& pos = graph.coordinates.at(node);
            routePath.append(QVariant::fromValue(
                QGeoCoordinate(pos.breddegrad, pos.lengdegrad)
            ));
        }

        std::reverse(routePath.begin(), routePath.end());
    }

    QQuickWidget window;
    auto nameForNode = [&](int node) -> QString {
        auto it = pointsOfInterest.find(node);
        if (it != pointsOfInterest.end()) {
            return QString::fromStdString(it->second.navn);
        }
        return QString("Node %1").arg(node);
    };

    window.setInitialProperties({
        {"routePath", routePath},
        {"startName", nameForNode(source)},
        {"goalName", nameForNode(goal)}
    });
    window.setSource(QUrl("qrc:/MapView.qml"));
    window.setWindowTitle("Rutekart");
    window.setResizeMode(QQuickWidget::SizeRootObjectToView);
    window.resize(1000, 700);
    window.setSource(QUrl("qrc:/MapView.qml"));
    window.show();

    return app.exec();
}

