#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <numbers>
#include <queue>

class Graph {
public:
    int nodeCount;

    struct Coordinates {
        double breddegrad;
        double lengdegrad;
    };

    std::vector<Coordinates> coordinates;

    struct Edge {
        int to;
        int weight;
    };

    std::map<int, std::vector<Edge>> adjList;

    void add_edge(int u, int v, int weight) {
        adjList[u].push_back({v, weight});
        // adjList[v].push_back({u, weight});
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
            long long kandidat = qDist + weight;

            if (kandidat < dist[neighbourVertex]) {
                dist[neighbourVertex] = qDist + weight;
                min_pq.push({dist[neighbourVertex], neighbourVertex});
            }
        }
    }

    return dist;
}

double haversine(double b1, double b2, double l1, double l2) {
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

std::vector<int> aStjerne(const Graph& graph, int src, int goal, double maxSpeedKmh) {
    using QueueEntry = std::tuple<double, int, int>;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> min_pq;

    const auto& [goalLat, goalLon] = graph.coordinates.at(goal);

    auto h = [&](int node) {
        const auto& [lat, lon] = graph.coordinates.at(node);
        return haversine(lat, goalLat, lon, goalLon) / maxSpeedKmh * 360000.0;
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

            int nyTid = qDist + weight;

            if (nyTid < dist[neighbourVertex]) {
                dist[neighbourVertex] = nyTid;
                min_pq.emplace(nyTid + h(neighbourVertex), nyTid, neighbourVertex);
            }
        }
    }

    return dist;
}

std::vector<int> alt(const Graph& graph, int src, int goal, const std::vector<std::vector<int>>& landmarkDistances) {
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

void initGraph(Graph &graph, std::string filePath) {
    std::cout << "Initializing Graph..." << '\n';

    std::ifstream nodeData(filePath);

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
            throw std::runtime_error("Ugyldig noderad i " + filePath);
        }

        graph.coordinates[nodenr] = position;
        graph.adjList.try_emplace(nodenr);  // Opprett en tom naboliste
    }

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

struct InteressePunkt {
    int nodenr;
    int kode;
    std::string navn;
};

void initInteressePkt(std::vector<InteressePunkt> &interessePunkter, std::string filePath) {
    std::ifstream interessePunktData(filePath);

    if (!interessePunktData) {
        std::cerr << "Could not read the node count from data/interessepkt.txt\n";
    }

    int interesseCount;
    interessePunktData >> interesseCount;
    std::cout << "Antall Interesse Punkter: " << interesseCount << '\n';


    int nodenr, kode;
    std::string navn;
    while (interessePunktData >> nodenr >> kode >> std::quoted(navn)) {
        interessePunkter.emplace_back(nodenr, kode, navn);
    }

    int pktCount = 0;
    for (const auto& pkt : interessePunkter) {
        if (pktCount >= 50) break;

        if (pkt.navn == "Subway") {
            std::cout << "Found it";
            break;
        }

        std::cout << "nodenr: " << pkt.nodenr << ", kode: " << pkt.kode << ", navn: " << pkt.navn << '\n';

        ++pktCount;
    }
}

int main(int argc, char *argv[]) {
    // Stjørdal node: 2114236
    // Steinkjer node: 2044600

    // 3942 64.1303419 -21.8707187 Domino's
    // 62192 64.3215983 -22.0701350 Subway
    double result = haversine(64.1303419, 64.3215983, -21.8707187, -22.0701350);
    std::cout << "Haversine result: " << result << '\n';

    std::vector<InteressePunkt> interessePunkter;
    initInteressePkt(interessePunkter, "data/interessepkt.txt");

    Graph graph;
    initGraph(graph, "data/noder.txt");

    std::vector<int> landmarkIds {
        559,
        1494,
        21427,
        53876,
        111354
    };

    std::vector<std::vector<int>> landmarkDistances;
    landmarkDistances.reserve(landmarkIds.size());

    for (int landmarkId : landmarkIds) {
        landmarkDistances.push_back(dijkstra(graph, landmarkId, -1));
    }

    int source = 3942;
    int goal = 62192;

    clock_t before = clock();
    std::vector<int> dist = dijkstra(graph, source, goal);
    clock_t duration = clock() - before;

    std::cout << "Duration (Dijkstra's): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    float tid = static_cast<float>(dist[goal]) / 100 / 60;
    std::cout << "Tid (bil) (tt.mm.ss): " << tid << '\n';
    std::cout << '\n';

    before = clock();
    dist = aStjerne(graph, source, goal, 45);
    duration = clock() - before;

    std::cout << "Duration (Astjerne): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    tid = static_cast<float>(dist[goal]) / 100 / 60;
    std::cout << "Tid (bil) (tt.mm.ss): " << tid << '\n';
    std::cout << '\n';

    before = clock();
    dist = alt(graph, source, goal, landmarkDistances);
    duration = clock() - before;

    std::cout << "Duration (ALT): " << static_cast<float>(duration) / CLOCKS_PER_SEC << " seconds" << '\n';

    tid = static_cast<float>(dist[goal]) / 100 / 60;
    std::cout << "Tid (bil) (tt.mm.ss): " << tid << '\n';
    std::cout << '\n';

    return 0;
}

