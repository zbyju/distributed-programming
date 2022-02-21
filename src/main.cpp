#include <iostream>

#include "command-line-args.h"

int main(int argc, char *argv[]) {
  std::cout << "Hello World!" << std::endl;
  CommandLineArgs::printHelp();
  return 0;
}
