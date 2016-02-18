#include "hitmiss.h"

HitMiss::HitMiss()
{
  Generator.seed(time(NULL));
  RandomRealdistribution = std::uniform_real_distribution<double>(-1.0,1.0);

  RandomDouble = std::bind(RandomRealdistribution, Generator);
}

HitMiss::~HitMiss() {}

double HitMiss::Simulate(uint32_t N)
{
  double x, y;
  for (uint32_t i = 0; i < N; i++) {
    x = RandomDouble();
    y = RandomDouble();
    if (pow(x, 2) + pow(y, 2) < 1) { Hits++; }
  }
  Total += N;
  return 4.0*Hits/Total;
}

double HitMiss::Simulate(double error)
{
  double pi = 0.0;
  for (uint32_t i = ErrorInit; i < ErrorLimit; i++) {
    pi = Simulate(static_cast<uint32_t>(1 << i));
    if (std::abs(pi - RealPi) < error) {
      printf("N: %d\n", Total);
      return pi;
    }
  }
  return pi;
}
