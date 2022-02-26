#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "node.h"

class Graph {
 private:
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<std::shared_ptr<Edge>> edges;
  uint8_t n;
  uint8_t k;

  void addNNodes(uint8_t n);

 public:
  Graph(uint8_t n);
  Graph(uint8_t n, uint8_t k);

  std::vector<std::shared_ptr<Node>>& getNodes();
  std::vector<std::shared_ptr<Edge>>& getEdges();
  uint8_t getN();
  uint8_t getK();

  /**
   * @brief Adds an edge to the graph. The graph is undirected; meaning that the
   * order of nodes (from -> to vs to -> from) doesnt matter.
   *
   * @param id1 - one of the nodes
   * @param id2 - the other node
   * @param weight - positive weight of the edge; 0 means it won't be added
   */
  void createEdge(uint8_t id1, uint8_t id2, uint32_t weight);

  void init();

  /**
   * @brief Checks if the graph is connected and bipartite (by trying to
   * color it with 2 colors)
   *
   * @return std::pair<bool, bool> where the first bool is true when the
   * graph is conntected and the second is true when it is bipartite
   */
  std::pair<bool, bool> isConnectedBipartite() const;

  std::shared_ptr<Node> findNodeById(uint8_t id) const;

  std::shared_ptr<Edge> findEdgeByIds(uint8_t id1, uint8_t id2);

  std::shared_ptr<Edge> findEdgeWithMaxWeight();

  friend std::ostream& operator<<(std::ostream&, const Graph&);
  std::string matrixToString();
  std::string edgesToString();
};
#endif
