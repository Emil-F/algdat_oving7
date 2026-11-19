//
// Created by Emil-PC on 19.11.2026.
//

#ifndef OVING7_ALGO_H
#define OVING7_ALGO_H
#include <vector>
#include "init.h"

namespace Algorithm {
    std::vector<int> dijkstra(const Init::Graph& graph, int src, int goal);
    std::vector<int> alt(const Init::Graph& graph, int src, int goal, const std::vector<std::vector<int>>& landmarkDistances);
    double haversine(double b1, double b2, double l1, double l2);
}

#endif //OVING7_ALGO_H
