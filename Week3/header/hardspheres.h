#pragma once
#include "shared.h"

class HardSpheres
{
private:
  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealDistribution;
  std::uniform_int_distribution<uint32_t> RandomIntDistribution;
  std::function<double()> RandomDouble;

  uint32_t Dimensions = 0, SpheresNumber = 0, SpheresPerDim = 0, * LocCoefs= nullptr, SavedSteps = 0;
  double SphereSize = 0.0, StepSize = 0.0, PackFraction = 0.0;

  Point * Spheres= nullptr, * Basis= nullptr, * SphereCursor= nullptr, * SystemSize= nullptr,  SystemSizeHalf, ** SpheresStored= nullptr;

  void __LocateSpheres(uint32_t d);
  bool __Overlap(const Point & s1, const Point & s2);

public:
  HardSpheres();
  ~HardSpheres();

  Point * GenerateWithBasis(uint32_t dim, Point * basis, uint32_t sph_per_dim, double sph_size);
  Point * GenerateRectangular(uint32_t dim, uint32_t sph_per_dim, double sph_size, double scale = 2.0);
  Point * GenerateFromFile(char const * filename);

  void ComputeSystemSize();
  void UpdateParticles(double delta, uint32_t steps, uint32_t save_step = 1);

  void LoadSpheres(char const * filename);
  void SaveSpheres(char const * filename);
  void SaveStoredSpheres(char const * filename);
};
