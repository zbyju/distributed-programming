#ifndef SOLVER_H
#define SOLVER_H

#include <ctime>
#include <memory>

#include "graph.h"

class Solver {
 private:
  std::shared_ptr<Graph> graph;
  uint32_t recursionCount;
  clock_t calculationStart;
  clock_t calculationEnd;
  unsigned int maxWeight;

  Graph solveDFS() const;

 public:
  Solver(std::shared_ptr<Graph> graph);

  double getCalculationTime() const;

  void solve();
};

#endif
