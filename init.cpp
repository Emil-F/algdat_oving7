//
// Created by Emil-PC on 19.11.2026.
//
#include "init.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

#include "algorithm.h"

void Init::Graph::add_edge(int u, int v, int weight) {
    adjList[u].push_back({.to = v, .weight = weight});
    adjList[v].push_back({.to = u, .weight = weight});
}

void Init::Graph::print(int limit)
{
    std::cout << "Adjacency list for the Graph: " << '\n';
    int count = 0;

    for (const auto& [vertex, edges] : adjList) {
        if (count >= limit) break;

        std::cout << vertex << " -> ";
        for (const auto& [to, weight] : edges) {
            std::cout << to << " (weight: " << weight << ") ";
        }
        std::cout << '\n';
        count++;
    }
}

void Init::initGraph(Graph &graph, std::string noderFilePath, std::string kanterFilePath) {
    std::cout << "Initializing Graph..." << '\n';

    std::ifstream nodeData(noderFilePath);

    if (!nodeData) {
        std::cerr << "Could not read the node count from data/noder.txt\n";
    }

    nodeData >> graph.nodeCount;

    graph.coordinates.resize(graph.nodeCount);
    for (int i = 0; i < graph.nodeCount; i++) {
        int nodenr;
        Graph::Coordinates position;

        if (!(nodeData >> nodenr >> position.breddegrad >> position.lengdegrad)
            || nodenr < 0 || nodenr >= graph.nodeCount) {
            throw std::runtime_error("Ugyldig noderad i " + noderFilePath);
        }

        graph.coordinates[nodenr] = position;
        graph.adjList.try_emplace(nodenr);  // Opprett en tom naboliste
    }

    std::cout << "Node count: " << graph.nodeCount << '\n';

    nodeData.close();

    std::ifstream kantData(kanterFilePath);

    std::size_t edgeCount;
    kantData >> edgeCount;

    int franode, tilnode, kjoretid, lengde, fartsgrense;
    while (kantData >> franode >> tilnode >> kjoretid >> lengde >> fartsgrense) {
        graph.add_edge(franode, tilnode, kjoretid);
    }

    std::cout << "Initialization Finished";
    std::cout << '\n';
    std::cout << '\n';
}

void Init::initInterestPoints(std::vector<InterestPoint> &interestPoints, std::string filePath) {
    std::ifstream interessePunktData(filePath);

    if (!interessePunktData) {
        std::cerr << "Could not read the node count from " + filePath + "\n";
    }

    std::cout << "Initializing interessepunkter..." << '\n';

    int interesseCount;
    interessePunktData >> interesseCount;
    std::cout << "Antall Interesse Punkter: " << interesseCount << '\n';


    int nodenr, kode;
    std::string navn;
    while (interessePunktData >> nodenr >> kode >> std::quoted(navn)) {
        interestPoints.emplace_back(nodenr, kode, navn);
    }

    std::cout << "Finished initializing interessepunkter" << '\n';
    std::cout << '\n';
}

void Init::processLandmarkDistances(Graph &graph, std::vector<std::vector<int>> &landmarkDistances, std::vector<int> &landmarkIds) {
    for (int landmarkId : landmarkIds) {
        landmarkDistances.push_back(Algorithm::dijkstra(graph, landmarkId, -1));
    }

    for (std::size_t i = 0; i < landmarkIds.size(); ++i) {
        std::ofstream file("data/landmark/landmark_" + std::to_string(landmarkIds[i]) + ".bin",
                           std::ios::binary);

        const auto& distances = landmarkDistances[i];
        file.write(reinterpret_cast<const char*>(distances.data()),
                   distances.size() * sizeof(int));
    }
}

void Init::readLandmarkDistances(
    Graph &graph,
    std::vector<std::vector<int>> &landmarkDistances,
    std::vector<int> &landmarkIds
    ) {

    std::cout << "Leser landmark distances" << '\n';

    for (int landmarkId : landmarkIds) {
        std::string path = "data/landmark/landmark_" + std::to_string(landmarkId) + ".bin";
        std::ifstream file(path, std::ios::binary);

        if (!file) {
            std::cerr << "Kunne ikke åpne " << path << '\n';
        }

        std::vector<int> distances(graph.nodeCount);
        const auto bytes = static_cast<std::streamsize>(distances.size() * sizeof(int));

        if (!file.read(reinterpret_cast<char*>(distances.data()), bytes)) {
            std::cerr << "Kunne ikke lese alle kjøretidene fra " << path << '\n';
        }

        landmarkDistances.push_back(std::move(distances));
    }
}