#include "hitmiss.h"

HitMiss::HitMiss()
{
  Generator.seed(DefaultClock::now().time_since_epoch().count());
  RandomRealDistribution = std::uniform_real_distribution<double>(-1.0,1.0);

  RandomDouble = std::bind(RandomRealDistribution, Generator);
}

HitMiss::~HitMiss() {}

double HitMiss::Simulate(uint32_t N)
{
  double x, y;
  for (uint32_t i = 0; i < N; i++) {
    x = RandomDouble();
    y = RandomDouble();
    if (x*x + y*y < 1) { Hits++; }
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
      printf("Points used: %d\n", Total);
      return pi;
    }
  }
  return pi;
}
