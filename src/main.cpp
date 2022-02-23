#include <iostream>

#include "command-line-args.h"
#include "input_parser.h"
#include "node.h"

int main(int argc, char *argv[]) {
  std::unique_ptr<ProgramArgs> args_ptr =
      CommandLineArgs::parseOptions(argc, argv);
  if (!args_ptr->getSuccess()) {
    CommandLineArgs::printHelp();
    return 0;
  }

  InputParser ip = InputParser(args_ptr->getInputPath());

  std::cout << *ip.getGraph() << std::endl;
  std::cout << ip.getGraph()->matrixToString() << std::endl;
  return 0;
}
