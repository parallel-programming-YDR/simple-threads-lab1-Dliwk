#ifndef LAB1_APP_H
#define LAB1_APP_H

#include <chrono>
#include <string>

class App
{
public:
  App(int argc, char** argv);

  App(const App&) = delete;
  App(App&&)      = delete;

  App& operator=(const App&) = delete;
  App& operator=(App&&)      = delete;

  void Run();

private:
  int         argc_;
  char**      argv_;
  const char* appname_ = "lab";

  void printUsage();
  void parseArguments(int& tries, int& seed);
  void printError(std::string error);
  auto calculateArea(int tries, int seed, int radius, int nthreads) -> std::pair<std::chrono::microseconds, double>;
};

#endif // LAB1_APP_H