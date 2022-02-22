#include "input_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

InputParser::InputParser(const std::string& inputPath) : inputPath(inputPath) {
  std::ifstream infile(inputPath);
  std::string line;
  std::vector<std::string> lines;
  while (std::getline(infile, line)) {
    lines.push_back(line);
  }
  for (auto line : lines) {
    std::cout << line << std::endl;
  }
}
