#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <memory>
#include <string>

#include "graph.h"

class InputParser {
 private:
  std::string inputPath;

 public:
  InputParser(const std::string& inputPath);
  std::unique_ptr<Graph> getGraph() const;
};

#endif
