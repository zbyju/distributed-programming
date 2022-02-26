#ifndef NODE_H
#define NODE_H

#include <memory>
#include <vector>

#include "edge.h"

class Node {
 private:
  uint8_t id;
  std::vector<std::shared_ptr<Edge>> edges;

 public:
  Node(uint8_t id);

  uint8_t getId() const;
  std::vector<std::shared_ptr<Edge>>& getEdges();
  void addEdge(std::shared_ptr<Edge> e);
  friend std::ostream& operator<<(std::ostream&, const Node&);
  std::string edgesToString() const;
};

#endif
