#ifndef COMMAND_LINE_ARGS_H
#define COMMAND_LINE_ARGS_H

#include <memory>
#include <string>

class ProgramArgs {
 private:
  const bool success;
  const uint8_t n;
  const uint8_t k;
  const std::string inputPath;

 public:
  ProgramArgs();
  ProgramArgs(uint8_t, uint8_t, std::string&);
  const uint8_t getN() const;
  const uint8_t getK() const;
  const std::string& getInputPath() const;
  const bool getSuccess() const;
  friend std::ostream& operator<<(std::ostream&, const ProgramArgs&);
};

class CommandLineArgs {
 public:
  static void printHelp();

  static std::unique_ptr<ProgramArgs> parseOptions(int argc, char* argv[]);
};

#endif
