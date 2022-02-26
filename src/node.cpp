#include "node.h"

#include <iostream>

Node::Node(uint8_t id) : id(id) {}

uint8_t Node::getId() const { return this->id; }
std::vector<std::shared_ptr<Edge>>& Node::getEdges() { return this->edges; }
void Node::addEdge(std::shared_ptr<Edge> e) {
  if (e->getFrom() != this->id) return;
  edges.push_back(e);
}

std::ostream& operator<<(std::ostream& s, const Node& n) {
  return s << "Node(" << n.id << "): #edges: " << n.edges.size();
}

std::string Node::edgesToString() const {
  std::string res = "";
  for (auto e : this->edges) {
    res += "Edge from: " + std::to_string(e->getFrom());
    res += "; to: " + std::to_string(e->getTo());
    res += "; weight: " + std::to_string(e->getWeight());
    res += "\n";
  }
  return res;
}
