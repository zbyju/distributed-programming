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
};

#endif
