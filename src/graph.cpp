#include "graph.h"

#include <stdint.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

#include "edge.h"

void Graph::addNNodes(uint8_t n) {
  for (int i = 0; i < n; ++i) {
    std::shared_ptr<Node> n = std::make_shared<Node>(i);
    this->nodes.push_back(n);
  }
}

Graph::Graph(uint8_t n) : n(n), k(0) { this->addNNodes(n); }
Graph::Graph(uint8_t n, uint8_t k) : n(n), k(k) { this->addNNodes(n); }

std::ostream& operator<<(std::ostream& s, const Graph& g) {
  return s << "Graph n:" << g.n << "; k: " << g.k << "; #edge" << g.edges.size()
           << "; #nodes: " << g.nodes.size();
}

std::vector<std::shared_ptr<Node>>& Graph::getNodes() { return this->nodes; }
std::vector<std::shared_ptr<Edge>>& Graph::getEdges() { return this->edges; }
uint8_t Graph::getN() { return this->n; }
uint8_t Graph::getK() { return this->k; }

void Graph::createEdge(uint8_t id1, uint8_t id2, uint32_t weight) {
  std::shared_ptr<Edge> e1 = std::make_shared<Edge>(id1, id2, weight);
  std::shared_ptr<Edge> e2 = std::make_shared<Edge>(id2, id1, weight);
  this->edges.push_back(e1);
  this->edges.push_back(e2);
  std::shared_ptr<Node> n1 = this->findNodeById(id1);
  std::shared_ptr<Node> n2 = this->findNodeById(id2);
  n1->addEdge(e1);
  n1->addEdge(e2);
  n2->addEdge(e1);
  n2->addEdge(e2);
}

std::shared_ptr<Node> Graph::findNodeById(uint8_t id) const {
  for (std::shared_ptr<Node> n : this->nodes) {
    if (n->getId() == id) {
      return n;
    }
  }
  return nullptr;
}

std::shared_ptr<Edge> Graph::findEdgeByIds(uint8_t from, uint8_t to) {
  for (auto e : this->edges) {
    if (e->getFrom() == from && e->getTo() == to) return e;
  }
  return nullptr;
}

std::shared_ptr<Edge> Graph::findEdgeByIdsSymmetric(uint8_t id1, uint8_t id2) {
  for (auto e : this->edges) {
    if (e->getFrom() == id1 && e->getTo() == id2) return e;
    if (e->getFrom() == id2 && e->getTo() == id1) return e;
  }
  return nullptr;
}

std::shared_ptr<Edge> Graph::findEdgeWithMaxWeight() {
  if (this->edges.size() == 0) return nullptr;
  std::shared_ptr<Edge> res = this->edges[0];
  for (auto e : this->edges) {
    if (e->getWeight() > res->getWeight()) res = e;
  }
  return res;
}

std::string Graph::matrixToString() {
  std::string res = "";
  uint8_t maxLength =
      std::to_string(this->findEdgeWithMaxWeight()->getWeight()).length();
  for (int i = 0; i < this->n; ++i) {
    for (int j = 0; j < this->n; ++j) {
      std::shared_ptr<Edge> e = this->findEdgeByIds(i, j);
      std::string weight = "0";
      if (e != nullptr) weight = std::to_string(e->getWeight());
      std::string padding = " ";
      while (padding.length() + weight.length() < maxLength + 1) padding += " ";
      res += weight + padding;
    }
    res += "\n";
  }
  return res;
}
