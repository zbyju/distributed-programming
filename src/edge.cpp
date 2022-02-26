#include "edge.h"

#include <stdint.h>

#include <iostream>

Edge::Edge(uint8_t from, uint8_t to, uint32_t weight)
    : from(from), to(to), weight(weight), state(undefined) {}

uint8_t Edge::getFrom() { return this->from; }
uint8_t Edge::getTo() { return this->to; }
uint8_t Edge::getOther(uint8_t id) {
  if (id == this->from)
    return this->to;
  else
    return this->from;
}
uint32_t Edge::getWeight() { return this->weight; }
EdgeState Edge::getState() { return this->state; }

std::ostream& operator<<(std::ostream& s, const Edge& e) {
  return s << "E(w = " << e.weight << ") from: " << e.from << " to: " << e.to;
}

bool Edge::sortByWeight(const std::shared_ptr<Edge> e1,
                        const std::shared_ptr<Edge> e2) {
  return e1->getWeight() > e2->getWeight();
}
