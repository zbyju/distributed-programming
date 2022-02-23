#include "edge.h"

#include <stdint.h>

#include <iostream>

Edge::Edge(uint8_t from, uint8_t to, uint32_t weight)
    : from(from), to(to), weight(weight) {}

uint8_t Edge::getFrom() { return this->from; }
uint8_t Edge::getTo() { return this->to; }
uint32_t Edge::getWeight() { return this->weight; }

std::ostream& operator<<(std::ostream& s, const Edge& e) {
  return s << "E(w = " << e.weight << ") from: " << e.from << " to: " << e.to;
}
