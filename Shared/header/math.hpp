#pragma once
#include <random>
#include <cmath>
#include <chrono>

#include "vector.hpp"

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

  ~RandomGenerator() {}

  double RandomDouble() { return RandomRealDistribution(Generator); }
  double RandomProb() { return RandomRealDistributionUnit(Generator); }
  uint32_t RandomInt() { return RandomIntDistribution(Generator); }

  void SetRange(uint32_t a, uint32_t b) {
    RandomIntDistribution = std::uniform_int_distribution<uint32_t>(a, b);
  }

  void SetRange(double a, double b) {
    RandomRealDistribution = std::uniform_real_distribution<double>(a, b);
  }

  template <uint32_t D> Vector<D> RandomVector(double a, double b) {
    Vector<D> result;
    for (uint32_t i = 0; i < D; i++) {
      result[i] = a + (b - a) * RandomProb();
    }
    return result;
  }

  template <uint32_t D> Vector<D> & RandomVector(double a, double b, Vector<D> & v) {
    for (uint32_t i = 0; i < D; i++) {
      v[i] = a + (b - a) * RandomProb();
    }
    return v;
  }

  template <uint32_t D> Vector<D> RandomVector(const Vector<D> & a, const Vector<D> & b) {
    Vector<D> result;
    for (uint32_t i = 0; i < D; i++) {
      result[i] = a[i] + (b[i] - a[i]) * RandomProb();
    }
    return result;
  }

  template <uint32_t D> Vector<D> & RandomVector(const Vector<D> & a, const Vector<D> & b, Vector<D> & v) {
    for (uint32_t i = 0; i < D; i++) {
      v[i] = a[i] + (b[i] - a[i]) * RandomProb();
    }
    return v;
  }
};
