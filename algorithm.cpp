//
// Created by Emil-PC on 19.11.2026.
//

#include "algorithm.h"

#include <cmath>
#include <iostream>
#include <queue>

std::vector<int> Algorithm::dijkstra(const Init::Graph& graph, int src, int goal) {
    // alias
    using QueueEntry = std::pair<int, int>;

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> min_pq;

    std::vector dist(graph.nodeCount, INT_MAX);

    // set distance from source to itself to 0
    dist[src] = 0;
    min_pq.push({0, src});

    int visitCount = 0;

    while (!min_pq.empty()) {
        auto [qDist, curV] = min_pq.top();
        min_pq.pop();

        if (qDist > dist[curV]) continue;

        ++visitCount;
        if (curV == goal) {
            std::cout << "Antall noder: " << visitCount << '\n';
            break;
        };

        for (const auto& edge : graph.adjList.at(curV)) {
            int neighbourVertex = edge.to;
            int weight = edge.weight;
            long long kandidat = qDist + weight;

            if (kandidat < dist[neighbourVertex]) {
                dist[neighbourVertex] = qDist + weight;
                min_pq.push({dist[neighbourVertex], neighbourVertex});
            }
        }
    }

    return dist;
}

double Algorithm::haversine(double b1, double b2, double l1, double l2) {
    constexpr double toRadians = std::numbers::pi / 180;

    b1 *= toRadians;
    b2 *= toRadians;
    l1 *= toRadians;
    l2 *= toRadians;

    constexpr double r = 6371; // Jordens radius
    const double term1 = pow(sin((b1 - b2)/2), 2);
    const double term2 = cos(b1)*cos(b2)*pow(sin((l1 - l2)/2), 2);

    return 2*r * asin(sqrt(term1 + term2));
}

std::vector<int> Algorithm::alt(const Init::Graph& graph, int src, int goal, const std::vector<std::vector<int>>& landmarkDistances) {
    using QueueEntry = std::tuple<double, int, int>;

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> min_pq;

    auto h = [&](int node) {
        int best = 0;

        for (const auto& distances : landmarkDistances) {
            int toNode = distances[node];
            int toGoal = distances[goal];

            if (toNode == INT_MAX || toGoal == INT_MAX) continue;

            best = std::max(best, std::abs(toGoal - toNode));
        }

        return best;
    };

    // alias
    std::vector dist(graph.nodeCount, INT_MAX);

    // set distance from source to itself to 0
    dist[src] = 0;

    min_pq.emplace(h(src), 0, src);

    int visitCount = 0;

    while (!min_pq.empty()) {
        auto [priority, qDist, curV] = min_pq.top();
        min_pq.pop();

        if (qDist > dist[curV]) continue;

        ++visitCount;
        if (curV == goal) {
            std::cout << "Antall noder: " << visitCount << '\n';
            break;
        };

        for (const auto& edge : graph.adjList.at(curV)) {
            int neighbourVertex = edge.to;
            int weight = edge.weight;

            const long long nyTid = static_cast<long long>(qDist) + weight;

            if (nyTid < dist[neighbourVertex]) {
                dist[neighbourVertex] = static_cast<int>(nyTid);
                min_pq.emplace(nyTid + h(neighbourVertex), nyTid, neighbourVertex);
            }
        }
    }

    return dist;
}