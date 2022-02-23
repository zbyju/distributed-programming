#ifndef EDGE_H
#define EDGE_H

#include <stdint.h>

#include <iostream>

class Edge {
 private:
  uint8_t from;
  uint8_t to;
  uint32_t weight;

 public:
  Edge(uint8_t from, uint8_t to, uint32_t weight);

  uint8_t getFrom();
  uint8_t getTo();
  uint32_t getWeight();

  friend std::ostream& operator<<(std::ostream&, const Edge&);
};

#endif
