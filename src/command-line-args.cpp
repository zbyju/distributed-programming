#include "command-line-args.h"

#include <iostream>

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
