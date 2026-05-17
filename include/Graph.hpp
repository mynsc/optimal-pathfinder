#pragma once
#include <vector>
#include <list>
#include <utility>

class Graph {
private:
    int nodes;
    std::vector<std::list<std::pair<int, int>>> adjacencyList;

public:
    Graph(int v);

    void addEdge(int u, int v, int weight);

    int getNodeCount() const;
    const std::list<std::pair<int, int>>& getNeighbors(int u) const;
};