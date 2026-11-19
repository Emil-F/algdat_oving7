//
// Created by Emil-PC on 19.11.2026.
//

#ifndef OVING7_ALGO_H
#define OVING7_ALGO_H
#include <vector>
#include "init.h"

namespace Algorithm {
    std::vector<int> Dijkstra(const Init::Graph& graph,
        int src,
        int goal,
        std::vector<int>* parent = nullptr
        );

    std::vector<int> ALT(
        const Init::Graph& graph,
        int src,
        int goal,
        const std::vector<std::vector<int>>& landmarkDistances,
        std::vector<int>* parent = nullptr
    );

    double haversine(double b1, double b2, double l1, double l2);

    std::vector<int> findNearestInterestPoints(
        const Init::Graph& graph,
        int src,
        std::unordered_map<int, Init::POI>& interestPoints,
        int kode,
        int limit
    );
}

#endif //OVING7_ALGO_H
