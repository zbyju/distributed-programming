#include <iostream>

#include "command-line-args.h"

int main(int argc, char *argv[]) {
  std::unique_ptr<ProgramArgs> args_ptr =
      CommandLineArgs::parseOptions(argc, argv);
  if (!args_ptr->getSuccess()) {
    CommandLineArgs::printHelp();
    return 0;
  }
  std::cout << *args_ptr << std::endl;
  return 0;
}
