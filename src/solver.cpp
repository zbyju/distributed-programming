#include "solver.h"

#include <ctime>
#include <memory>

enum Color {
  Undefined = 0,
  Red = 1,
  Blue = 2,
};

Solver::Solver(std::shared_ptr<Graph> graph) : graph(graph) {}

double Solver::getCalculationTime() const {
  return double(this->calculationEnd - this->calculationStart) / CLOCKS_PER_SEC;
}

Graph Solver::solveDFS() const {
  std::cout << this->graph->edgesToString() << std::endl;
  return *this->graph;
}

void Solver::solve() {
  this->calculationStart = clock();
  this->maxWeight = 0;
  this->graph->init();

  std::vector<Color> nodeColors;
  nodeColors.resize(this->graph->getN());

  

  this->solveDFS();

  this->calculationEnd = clock();
  std::cout << this->getCalculationTime() << std::endl;
}
