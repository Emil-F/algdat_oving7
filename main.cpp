
#include <ctime>
#include <iostream>

#include "algorithm.h"
#include "init.h"

int main(int argc, char *argv[]) {
    // Stjørdal node: 2114236
    // Steinkjer node: 2044600

    // 3942 64.1303419 -21.8707187 Domino's
    // 62192 64.3215983 -22.0701350 Subway
    std::unordered_map<int, Init::InterestPoint> interessePunkter;
    Init::initInterestPoints(interessePunkter, "data/island/interessepkt.txt");

    Init::Graph graph;
    Init::initGraph(graph, "data/island/noder.txt", "data/island/kanter.txt");

    std::vector<int> landmarkIds {
        2457,
        104613,
        18304,
        59613,
        79590
    };

    std::vector<std::vector<int>> landmarkDistances;

    Init::readLandmarkDistances(graph, landmarkDistances,  landmarkIds);

    int source = 4441;
    int goal = 103053;

    clock_t before = clock();
    std::vector<int> dist = Algorithm::Dijkstra(graph, source, goal);
    clock_t duration = clock() - before;

    std::cout << "Duration (Dijkstra's): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    float tid = static_cast<float>(dist[goal]) / 100 / 60;
    std::cout << "Tid (bil) (tt.mm.ss): " << tid << '\n';
    std::cout << '\n';

    before = clock();
    dist = Algorithm::ALT(graph, source, goal, landmarkDistances);
    duration = clock() - before;

    std::cout << "Duration (ALT): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    tid = static_cast<float>(dist[goal]) / 100 / 60;
    std::cout << "Tid (bil) (tt.mm.ss): " << tid << '\n';
    std::cout << '\n';

    before = clock();
    std::vector<int> interestPointsFound = Algorithm::findNearestInterestPoints(graph, source, interessePunkter, 16, 5);
    duration = clock() - before;

    std::cout << "Duration (Finn nærmeste interessepunkt): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    // tid = static_cast<float>(dist[goal]) / 100 / 60;
    // std::cout << "Tid (bil) (tt.mm.ss): " << tid << '\n';
    // std::cout << '\n';

    return 0;
}

