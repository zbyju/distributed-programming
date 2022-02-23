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

  void createEdge(uint8_t id1, uint8_t id2, uint32_t weight);

  std::shared_ptr<Node> findNodeById(uint8_t id) const;

  std::shared_ptr<Edge> findEdgeByIds(uint8_t from, uint8_t to);
  std::shared_ptr<Edge> findEdgeByIdsSymmetric(uint8_t id1, uint8_t id2);

  friend std::ostream& operator<<(std::ostream&, const Graph&);
  std::string matrixToString(uint8_t maxLength);
};
#endif
