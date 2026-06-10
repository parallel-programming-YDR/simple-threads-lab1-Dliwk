#include "app.hh"

#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

App::App(int const argc, char** const argv):
  argc_(argc),
  argv_(argv)
{
}

void App::Run()
{
  int tries = 0;
  int seed  = 0;
  parseArguments(tries, seed);

  int radius   = 0;
  int nthreads = 0;
  while (std::cin >> radius >> nthreads)
  {
    if (radius < 0)
    {
      printError("radius must be non-negative");
      exit(EXIT_FAILURE);
    }

    if (nthreads <= 0)
    {
      printError("nthreads must be positive");
      exit(EXIT_FAILURE);
    }

    auto [duration, area] = calculateArea(tries, seed, radius, nthreads);
    std::cout << std::fixed << std::setprecision(3) << duration.count() / 1000. << " " << area << std::endl;
  }
}

void App::parseArguments(int& tries, int& seed)
{
  if (argc_ > 0)
  {
    appname_ = argv_[0];
  }
  if (!(2 <= argc_ && argc_ <= 3))
  {
    printUsage();
    exit(EXIT_FAILURE);
  }

  try
  {
    tries = std::stoi(std::string(argv_[1]));
    if (argc_ > 2)
    {
      seed = std::stoi(std::string(argv_[2]));
    }
  }
  catch (std::exception&)
  {
    printError("parameters must be integers");
    exit(EXIT_FAILURE);
  }

  if (tries <= 0)
  {
    printError("tries must be positive");
    exit(EXIT_FAILURE);
  }

  if (seed < 0)
  {
    printError("seed must be non-negative");
    exit(EXIT_FAILURE);
  }
}

void App::printError(std::string const error)
{
  std::cerr << appname_ << ": " << error << std::endl;
}

void App::printUsage()
{
  std::cerr << "usage: " << appname_ << " tries [seed]" << std::endl;
}

static bool sample(std::mt19937& rng, int radius)
{
  std::uniform_real_distribution<double> distrib(-radius, radius);

  double x         = distrib(rng);
  double y         = distrib(rng);
  double distance2 = x * x + y * y;

  return distance2 < radius * radius;
}

auto App::calculateArea(int const tries, int const seed, int const radius, int const nthreads)
  -> std::pair<std::chrono::microseconds, double>
{

  using std::chrono::high_resolution_clock;

  auto start_time = high_resolution_clock::now();

  std::vector<std::thread> workers;
  std::vector<int>         successful_tries_per_worker(nthreads);
  std::mt19937             root_rng(seed);
  workers.reserve(nthreads);

  for (int i = 0; i < nthreads; ++i)
  {
    int      worker_tries     = tries / nthreads + (i < tries % nthreads ? 1 : 0);
    int&     successful_tries = successful_tries_per_worker[i];
    uint32_t worker_seed      = root_rng();

    workers.emplace_back(
      [worker_tries, &successful_tries, worker_seed, radius, this]
      {
        std::mt19937 rng{worker_seed};
        int          successful_worker_tries = 0;

        for (int i = 0; i < worker_tries; ++i)
        {
          if (sample(rng, radius))
          {
            successful_worker_tries += 1;
          }
        }

        successful_tries = successful_worker_tries;
      });
  }

  int successful_tries = 0;
  for (int i = 0; i < nthreads; ++i)
  {
    workers[i].join();
    successful_tries += successful_tries_per_worker[i];
  }

  double area = 1. * radius * radius * 4 * successful_tries / tries;

  auto duration = high_resolution_clock::now() - start_time;

  return {std::chrono::duration_cast<std::chrono::microseconds>(duration), area};
}