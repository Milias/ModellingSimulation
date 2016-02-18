#pragma once
#include "shared.h"

class HitMiss
{
private:
  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealdistribution;

  std::function<double()> RandomDouble;

  uint32_t Hits = 0, Total = 0, ErrorInit = 10, ErrorLimit = 32;

  double RealPi = 3.14159265359;

public:
  HitMiss();
  ~HitMiss();

  double Simulate(uint32_t N);
  double Simulate(double error);
};
