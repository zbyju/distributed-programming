#include "input_parser.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "edge.h"
#include "node.h"

InputParser::InputParser(const std::string& inputPath) : inputPath(inputPath) {
  std::ifstream infile(inputPath);
  std::string line;
  std::string firstLine;
  // Get N from the first line
  std::getline(infile, firstLine);
  uint8_t n = std::stoul(firstLine);

  this->graph = std::make_shared<Graph>(n);

  // Go through all the weights and add them to the graph
  std::vector<std::vector<uint32_t>> weights;
  int i = 0;
  int j = 0;
  while (std::getline(infile, line)) {
    std::istringstream buffer(line);
    for (std::string weightString; buffer >> weightString;) {
      uint32_t weight = std::stoul(weightString);
      if (weight != 0) {
        this->graph->createEdge(i, j, weight);
      }
      ++j;
    }
    j = 0;
    ++i;
  }
}

std::shared_ptr<Graph> InputParser::getGraph() { return this->graph; }
