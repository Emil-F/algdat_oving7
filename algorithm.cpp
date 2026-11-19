//
// Created by Emil-PC on 19.11.2026.
//

#include "algorithm.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>

std::vector<int> Algorithm::Dijkstra(const Init::Graph& graph, int src, int goal, std::vector<int>* parent) {
    if (parent) {
        parent->assign(graph.nodeCount, -1);
    }

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
            std::cout << "Antall uttak fra køen: " << visitCount << '\n';
            break;
        };

        for (const auto& edge : graph.adjList.at(curV)) {
            int neighbourVertex = edge.to;
            int weight = edge.weight;
            long long kandidat = qDist + weight;

            if (kandidat < dist[neighbourVertex]) {
                dist[neighbourVertex] = qDist + weight;

                if (parent) {
                    (*parent)[neighbourVertex] = curV;
                }

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

std::vector<int> Algorithm::ALT(const Init::Graph& graph, int src, int goal, const std::vector<std::vector<int>>& landmarkDistances) {
    using QueueEntry = std::tuple<double, int, int>;

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> min_pq;

    auto h = [&](int node) {
        int best = 0;

        for (std::size_t i = 0; i < landmarkDistances.size(); i += 2) {
            const auto& from = landmarkDistances[i];
            const auto& to   = landmarkDistances[i + 1];

            if (from[goal] != INT_MAX && from[node] != INT_MAX) {
                best = std::max(best, from[goal] - from[node]);
            }
            if (to[node] != INT_MAX && to[goal] != INT_MAX) {
                best = std::max(best, to[node] - to[goal]);
            }
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
            std::cout << "Antall uttak fra køen: " << visitCount << '\n';
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

std::vector<int> Algorithm::findNearestInterestPoints(
    const Init::Graph& graph,
    int src,
    std::unordered_map<int, Init::POI>& interestPoints,
    int intKode,
    int limit
    ) {
    using QueueEntry = std::pair<int, int>;

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> min_pq;

    std::vector dist(graph.nodeCount, INT_MAX);

    // set distance from source to itself to 0
    dist[src] = 0;
    min_pq.push({0, src});

    int visitCount = 0;

    std::vector<int> interestPointsFound;

    while (!min_pq.empty()) {
        auto [qDist, curV] = min_pq.top();
        min_pq.pop();

        if (qDist > dist[curV]) continue;

        ++visitCount;

        if (interestPointsFound.size() >= limit) break;

        for (const auto& edge : graph.adjList.at(curV)) {
            int neighbourVertex = edge.to;
            int weight = edge.weight;
            long long kandidat = qDist + weight;

            if (kandidat < dist[neighbourVertex]) {
                dist[neighbourVertex] = qDist + weight;
                min_pq.push({dist[neighbourVertex], neighbourVertex});
            }

            auto it = interestPoints.find(neighbourVertex);

            if (it != interestPoints.end() && !std::ranges::contains(interestPointsFound, neighbourVertex)) {
                int nodenr = it->first;
                int kode = it->second.kode;
                std::string navn = it->second.navn;

                if (kode == intKode) {
                    std::cout << "nodenr: " << nodenr << ", kode: " << kode << ", navn: " << navn << '\n';
                    interestPointsFound.push_back(nodenr);
                };
            }

        }
    }

    std::cout << "Antall uttak fra køen: " << visitCount << '\n';
    std::cout << "Antall steder: " << interestPointsFound.size() << '\n';

    return interestPointsFound;
}
