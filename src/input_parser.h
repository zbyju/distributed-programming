#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <memory>
#include <string>

#include "graph.h"

class InputParser {
 private:
  std::string inputPath;
  std::shared_ptr<Graph> graph;

 public:
  InputParser(const std::string& inputPath);
  std::shared_ptr<Graph> getGraph();
};

#endif
