//
// Created by Emil-PC on 19.11.2026.
//

#ifndef OVING7_INIT_H
#define OVING7_INIT_H
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace Init {
    class Graph {
    public:
        int nodeCount;

        struct Coordinates {
            double breddegrad;
            double lengdegrad;
        };

        struct Edge {
            int to;
            int weight;
        };

        std::vector<Coordinates> coordinates;
        std::vector<std::vector<Edge>> adjList;

        void add_edge(int u, int v, int weight);
        void print(int limit);
    };

    struct POI {
        int kode;
        std::string navn;
    };

    void initInterestPoints(std::unordered_map<int, POI>& interestPoints, std::string filePath);
    void initGraph(Graph &graph, std::string noderFilePath, std::string kanterFilePath);
    void processLandmarkDistances(Graph &graph, std::vector<std::vector<int>> &landmarkDistances, std::vector<int> &landmarkIds);
    void readLandmarkDistances(Graph &graph, std::vector<std::vector<int>> &landmarkDistances, std::vector<int> &landmarkIds);
}

#endif //OVING7_INIT_H

