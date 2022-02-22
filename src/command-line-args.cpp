#include "command-line-args.h"

#include <string.h>

#include <iostream>
#include <memory>

ProgramArgs::ProgramArgs() : success(false), n(0), k(0), inputPath("") {}
ProgramArgs::ProgramArgs(uint8_t n, uint8_t k, std::string& inputPath)
    : success(true), n(n), k(k), inputPath(inputPath) {}
const uint8_t ProgramArgs::getN() const { return this->n; }
const uint8_t ProgramArgs::getK() const { return this->k; }
const std::string& ProgramArgs::getInputPath() const { return this->inputPath; }
const bool ProgramArgs::getSuccess() const { return this->success; }

std::ostream& operator<<(std::ostream& s, const ProgramArgs& o) {
  return s << "n: " << unsigned(o.getN()) << "; k: " << unsigned(o.getK())
           << "; f: " << o.getInputPath();
}

void CommandLineArgs::printHelp() {
  std::cout << "Options:" << std::endl;

  std::cout << "\t -n \t [Required] \t Integer representing number of nodes."
            << std::endl;

  std::cout
      << "\t -k \t [Required] \t Integer representing the average node degree."
      << std::endl;

  std::cout << "\t -f \t [Required] \t Path to the file containing the graph."
            << std::endl;
}

std::unique_ptr<ProgramArgs> CommandLineArgs::parseOptions(int argc,
                                                           char* argv[]) {
  if (argc != 7) return std::make_unique<ProgramArgs>();
  uint8_t n = 0;
  uint8_t k = 0;
  std::string f = "";
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "-n") == 0) n = std::stoul(argv[i + 1]);
    if (strcmp(argv[i], "-k") == 0) k = std::stoul(argv[i + 1]);
    if (strcmp(argv[i], "-f") == 0) f = argv[i + 1];
  }
  if (n != 0 && k != 0 && f != "")
    return std::make_unique<ProgramArgs>(n, k, f);
  return std::make_unique<ProgramArgs>();
}
