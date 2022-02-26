#include "solver.h"

#include <ctime>
#include <memory>

Solver::Solver(std::shared_ptr<Graph> graph) : graph(graph) {}

double Solver::getCalculationTime() const {
  return double(this->calculationEnd - this->calculationStart) / CLOCKS_PER_SEC;
}

Graph Solver::solveDFS() const {
  for (int i = 0; i < 200000000; ++i) {
  }
  return *this->graph;
}

void Solver::solve() {
  this->calculationStart = clock();
  this->maxWeight = 0;
  this->graph

      this->solveDFS();

  this->calculationEnd = clock();
  std::cout << this->getCalculationTime() << std::endl;
}
