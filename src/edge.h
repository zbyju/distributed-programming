#ifndef EDGE_H
#define EDGE_H

#include <memory>

#include "node.h"

class Edge {
 private:
  std::shared_ptr<Node> from;
  std::shared_ptr<Node> to;
  uint32_t weight;

 public:
};

#endif
