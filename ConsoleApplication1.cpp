#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <set>

using namespace std;

// ===== Struktura dla krawêdzi =====
struct Edge {
    int u, v;

    Edge(int a, int b) {
        u = min(a, b);
        v = max(a, b);
    }

    bool operator==(const Edge& other) const {
        return u == other.u && v == other.v;
    }

    bool operator<(const Edge& other) const {
        return (u < other.u) || (u == other.u && v < other.v);
    }
};

// ===== Hash dla unordered_set/map =====
namespace std {
    template <>
    struct hash<Edge> {
        size_t operator()(const Edge& e) const {
            return hash<int>()(e.u) ^ hash<int>()(e.v << 1);
        }
    };
}

// ===== Wczytywanie grafu z pliku =====
unordered_map<int, unordered_set<int>> loadGraphFromFile(const string& filename) {
    unordered_map<int, unordered_set<int>> graph;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        int node;
        char colon;
        ss >> node >> colon;
        string rest;
        getline(ss, rest);
        stringstream neighbors(rest);
        int neighbor;
        while (neighbors >> neighbor) {
            graph[node].insert(neighbor);
            graph[neighbor].insert(node); // nieskierowany
            if (neighbors.peek() == ',') neighbors.ignore();
        }
    }
    return graph;
}

// ===== Lista unikalnych krawêdzi =====
vector<Edge> getEdges(const unordered_map<int, unordered_set<int>>& graph) {
    set<Edge> edges;
    for (const auto& [u, neighbors] : graph) {
        for (int v : neighbors) {
            edges.insert(Edge(u, v));
        }
    }
    return vector<Edge>(edges.begin(), edges.end());
}

// ===== Budowa grafu dualnego =====
vector<unordered_set<int>> buildDualGraph(const vector<Edge>& edges) {
    int n = edges.size();
    vector<unordered_set<int>> dual(n);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (edges[i].u == edges[j].u || edges[i].u == edges[j].v ||
                edges[i].v == edges[j].u || edges[i].v == edges[j].v) {
                dual[i].insert(j);
                dual[j].insert(i);
            }
        }
    }

    return dual;
}

// ===== Dijkstra =====
int dijkstra(const vector<unordered_set<int>>& dual, int start, int P) {
    vector<int> dist(dual.size(), INT32_MAX);
    dist[start] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    pq.emplace(0, start);

    int count = 0;

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > P) continue;
        count++;
        for (int v : dual[u]) {
            if (dist[v] > d + 1) {
                dist[v] = d + 1;
                pq.emplace(dist[v], v);
            }
        }
    }

    return count - 1; // odejmujemy sztuczn¹ krawêdŸ
}

// ===== G³ówna funkcja =====
int main() {
    int P = {};
    string filename = "graf.txt";

    cout << "Podaj maksymalny zasieg P: \n";
    cin >> P;

    auto graph = loadGraphFromFile(filename);
    auto edges = getEdges(graph);

    // Dodaj sztuczn¹ krawêdŸ do pierwszego wierzcho³ka
    int startVertex = graph.begin()->first;
    edges.push_back(Edge(-1, startVertex));
    int startEdgeIndex = edges.size() - 1;

    auto dual = buildDualGraph(edges);
    int result = dijkstra(dual, startEdgeIndex, P);

    cout << "Liczba osiagalnych krawedzi (dla P = " << P << "): " << result << endl;
    system("pause");
    return 0;
}
