#ifndef EDGE_H
#define EDGE_H

#include <stdint.h>

#include <iostream>
#include <memory>

enum EdgeState { undefined = -1, included = 1, excluded = 0 };

class Edge {
 private:
  uint8_t from;
  uint8_t to;
  uint32_t weight;
  EdgeState state;

 public:
  Edge(uint8_t from, uint8_t to, uint32_t weight);

  uint8_t getFrom();
  uint8_t getTo();
  uint8_t getOther(uint8_t id);
  uint32_t getWeight();
  EdgeState getState();

  friend std::ostream& operator<<(std::ostream&, const Edge&);
  static bool sortByWeight(const std::shared_ptr<Edge> e1,
                           const std::shared_ptr<Edge> e2);
};

#endif
