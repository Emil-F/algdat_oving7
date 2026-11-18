#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <queue>

class Graph {
public:
    int nodeCount;

    struct Edge {
        int to;
        int weight;
    };

    std::map<int, std::vector<Edge>> adjList;

    void add_edge(int u, int v, int weight) {
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
    }

    void print(int limit)
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
};

std::vector<int> dijkstra(const Graph& graph, int src, int goal) {
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

            if (dist[neighbourVertex] > qDist + weight) {
                dist[neighbourVertex] = qDist + weight;
                min_pq.push({dist[neighbourVertex], neighbourVertex});
            }
        }
    }

    return dist;
}

void initGraph(Graph &graph, std::string filePath) {
    std::cout << "Initializing Graph..." << '\n';

    std::ifstream nodeData(filePath);

    if (!nodeData) {
        std::cerr << "Could not read the node count from data/noder.txt\n";
    }

    nodeData >> graph.nodeCount;

    std::cout << "Node count: " << graph.nodeCount << '\n';

    nodeData.close();

    std::ifstream kantData("data/kanter.txt");

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

int main(int argc, char *argv[]) {
    // Stjørdal node: 2114236
    // Steinkjer node: 2044600

    Graph graph;
    initGraph(graph, "data/noder.txt");

    int source = 2800567;
    int goal = 7705656;

    clock_t before = clock();
    std::vector<int> dist = dijkstra(graph, source, goal);
    clock_t duration = clock() - before;

    std::cout << "Duration: " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    float tid = static_cast<float>(dist[goal]) / 100 / 60;
    std::cout << "Tid (bil) (tt.mm.ss): " << tid << '\n';

    return 0;
}

