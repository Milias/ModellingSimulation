#pragma once

//STL
#include <cstdint>
#include <cmath>
#include <cstring>
#include <chrono>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <cstdio>

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

// ThirdPary
#include "json.h"

#include "deprecated.h"

typedef std::chrono::high_resolution_clock DefaultClock;
typedef std::chrono::time_point<DefaultClock> t_point;

const double pi = 3.14159265359;

inline uint64_t factorial(uint64_t n) {
  if (n==0) {
    return 1;
  } else {
    return n*factorial(n-1);
  }
}

/*
  Convenience structure to manage step size
  adaptation to maintain roughfly 50% hit rate.
*/
struct StepSizeAdapter
{
  uint32_t hits = 0, count = 0;
  double rate = 0.0, delta = 0.0;
  double min = 0.0, max = 0.0;

  StepSizeAdapter(double delta0, double min_bound, double max_bound) : delta(delta0), min(min_bound), max(max_bound) {}

  double Update(bool sum) {
    if (sum) { hits++; }
    count++;
    rate = 1.0 * hits / (count + 1);
    delta *= 1.0 + (rate - 0.5)/std::pow(count,0.5);
    delta = delta < min ? min : (delta > max ? max : delta);
    return delta;
  }
};

class RandomGenerator
{
private:
  /*
    Random number generators.
  */
  std::default_random_engine Generator;

  /*
    Random number distributions.
  */
  std::uniform_real_distribution<double> RandomRealDistribution;
  std::uniform_real_distribution<double> RandomRealDistributionUnit;
  std::uniform_int_distribution<uint32_t> RandomIntDistribution;

public:
  RandomGenerator() {
    Generator.seed(DefaultClock::now().time_since_epoch().count());

    RandomRealDistribution = std::uniform_real_distribution<double>(-1.0,1.0);
    RandomRealDistributionUnit = std::uniform_real_distribution<double>(0.0,1.0);
  }

  ~RandomGenerator();

  double RandomDouble() { return RandomRealDistribution(Generator); }
  double RandomProb() { return RandomRealDistributionUnit(Generator); }
  uint32_t RandomInt() { return RandomIntDistribution(Generator); }

  void SetRange(uint32_t a, uint32_t b) {
    RandomIntDistribution = std::uniform_int_distribution<uint32_t>(a, b);
  }

  void SetRange(double a, double b) {
    RandomRealDistribution = std::uniform_real_distribution<double>(a, b);
  }
};
