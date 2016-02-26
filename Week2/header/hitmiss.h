#pragma once
#include "shared.h"

/*
  Class for computing Pi using the hit and miss Monte
  Carlo method.

  Defines a random number generator (defaulting in g++
  to mt19937 or mt19937_64), and a convenience function.

  The only two methods, Simulate(uint32_t) and Simulate(double),
  both return Pi computed by the hit and miss method.
*/
class HitMiss
{
private:
  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealDistribution;
  std::function<double()> RandomDouble;

  uint32_t Hits = 0, Total = 0, ErrorInit = 10, ErrorLimit = 32;

  double RealPi = 3.14159265359;

public:
  HitMiss();
  ~HitMiss();

  /*
    This method takes an integer N and generates
    that number of points in a square [-1,1]x[-1,1],
    counting how many of them are inside of a circle
    of radius one.

    Finally, Pi is approximated by computing 4.0*Hits/Total.
  */
  double Simulate(uint32_t N);

  /*
    This method, unlike the previous one, takes a double
    and interprets it as an error. Then, Simulate(uint32_t N)
    is executed with increasingly larger values of N, until
    Pi is approximated with an error less than epsilon.
  */
  double Simulate(double epsilon);
};
