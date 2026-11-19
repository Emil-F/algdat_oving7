//
// Created by Emil-PC on 19.11.2026.
//
#include "init.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <unordered_map>

#include "algorithm.h"

void Init::Graph::add_edge(int u, int v, int weight) {
    adjList[u].push_back({.to = v, .weight = weight});
    //adjList[v].push_back({.to = u, .weight = weight});
}

void Init::Graph::print(int limit)
{
    // std::cout << "Adjacency list for the Graph: " << '\n';
    // int count = 0;
    //
    // for (const auto& [vertex, edges] : adjList) {
    //     if (count >= limit) break;
    //
    //     std::cout << vertex << " -> ";
    //     for (const auto& [to, weight] : edges) {
    //         std::cout << to << " (weight: " << weight << ") ";
    //     }
    //     std::cout << '\n';
    //     count++;
    // }
}

void Init::initGraph(Graph &graph, std::string noderFilePath, std::string kanterFilePath) {
    std::cout << "Starter initGraph... (Kan ta litt tid)" << '\n';

    std::ifstream nodeData(noderFilePath);

    if (!nodeData) {
        std::cerr << "Could not read the node count from data/noder.txt\n";
    }

    nodeData >> graph.nodeCount;

    graph.coordinates.resize(graph.nodeCount);
    graph.adjList.resize(graph.nodeCount);

    int noderRowsRead = 0;
    for (int i = 0; i < graph.nodeCount; i++) {
        int nodenr;
        Graph::Coordinates position;

        if (!(nodeData >> nodenr >> position.breddegrad >> position.lengdegrad)
            || nodenr < 0 || nodenr >= graph.nodeCount) {
            throw std::runtime_error("Ugyldig noderad i " + noderFilePath);
        }

        graph.coordinates[nodenr] = position;
        ++noderRowsRead;
    }

    std::cout << "Noder Rows read: " << noderRowsRead << " of " << graph.nodeCount << '\n';

    nodeData.close();

    std::ifstream kantData(kanterFilePath);

    std::size_t edgeCount;
    kantData >> edgeCount;

    int kanterRowsRead = 0;
    int franode, tilnode, kjoretid, lengde, fartsgrense;
    while (kantData >> franode >> tilnode >> kjoretid >> lengde >> fartsgrense) {
        if (franode < 0 || franode >= graph.nodeCount ||
            tilnode < 0 || tilnode >= graph.nodeCount) {
            throw std::runtime_error(
                "Invalid edge " + std::to_string(franode) + " -> " +
                std::to_string(tilnode) + " for graph with " +
                std::to_string(graph.nodeCount) + " nodes"
            );
        }

        graph.add_edge(franode, tilnode, kjoretid);
        ++kanterRowsRead;
    }

    std::cout << "Kanter Rows read: " << kanterRowsRead << " of " << edgeCount << '\n';

    std::cout << "Ferdig med initGraph";
    std::cout << '\n';
    std::cout << '\n';
}

void Init::initInterestPoints(std::unordered_map<int, POI>& interestPoints, std::string filePath) {
    std::ifstream interessePunktData(filePath);

    if (!interessePunktData) {
        std::cerr << "Could not read the node count from " + filePath + "\n";
    }

    std::cout << "Starter initInterestPoints..." << '\n';

    int interesseCount;
    if (!(interessePunktData >> interesseCount)) {
        throw std::runtime_error("Could not read POI count from " + filePath);
    };

    std::cout << "Antall Interesse Punkter: " << interesseCount << '\n';

    int rowsRead = 0;
    int nodenr, kode;
    std::string navn;
    while (interessePunktData >> nodenr >> kode) {
        std::getline(interessePunktData >> std::ws, navn);

        if (navn.size() >= 2 && navn.front() == '"' && navn.back() == '"') {
            navn = navn.substr(1, navn.size() - 2);
        }

        interestPoints.emplace(nodenr, POI{kode, navn});
        ++rowsRead;
    }

    if (interessePunktData.fail() && !interessePunktData.eof()) {
        interessePunktData.clear();
        std::string remaining;
        std::getline(interessePunktData, remaining);
        std::cerr << "Stopped reading near: [" << remaining << "]\n";
    }

    std::cout << "Rows read: " << rowsRead << " of " << interesseCount << '\n';
    std::cout << "Contains node 2488211: " << std::boolalpha
              << interestPoints.contains(2488211) << '\n';

    std::cout << "Ferdig med initInterestPoints" << '\n';
    std::cout << '\n';
}

void Init::processLandmarkDistances(
    Graph& graph,
    std::vector<std::vector<int>>& landmarkDistances,
    std::vector<int>& landmarkIds
) {
    Graph reversed;
    reversed.nodeCount = graph.nodeCount;
    reversed.adjList.resize(graph.nodeCount);

    for (int node = 0; node < graph.nodeCount; ++node) {
        for (const auto& edge : graph.adjList[node]) {
            reversed.add_edge(edge.to, node, edge.weight);
        }
    }

    landmarkDistances.clear();

    for (int id : landmarkIds) {
        std::string base = "data/landmark/landmark_" + std::to_string(id);

        auto saveRow = [&](const Graph& g, const std::string& path) {
            auto distances = Algorithm::Dijkstra(g, id, -1);

            std::ofstream file(path, std::ios::binary);
            file.write(reinterpret_cast<const char*>(distances.data()),
                       distances.size() * sizeof(int));
            file.close();
            if (!file) {
                throw std::runtime_error("Kunne ikke skrive " + path);
            }

            landmarkDistances.push_back(std::move(distances));
        };

        saveRow(graph,    base + "_from.bin");
        saveRow(reversed, base + "_to.bin");
    }
}

void Init::readLandmarkDistances(
    Graph& graph,
    std::vector<std::vector<int>>& landmarkDistances,
    std::vector<int>& landmarkIds
) {
    std::vector<std::vector<int>> loaded;

    for (int id : landmarkIds) {
        std::string base = "data/landmark/landmark_" + std::to_string(id);

        for (const char* suffix : {"_from.bin", "_to.bin"}) {
            std::string path = base + suffix;
            std::ifstream file(path, std::ios::binary);

            std::vector<int> distances(graph.nodeCount);
            const auto bytes = static_cast<std::streamsize>(
                distances.size() * sizeof(int)
            );

            if (!file.read(reinterpret_cast<char*>(distances.data()), bytes)) {
                throw std::runtime_error("Kunne ikke lese " + path);
            }

            loaded.push_back(std::move(distances));
        }
    }

    landmarkDistances = std::move(loaded);
}