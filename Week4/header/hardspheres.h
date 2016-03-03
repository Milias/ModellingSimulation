#pragma once
#include "shared.h"

class HardSpheres
{
private:
  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealDistribution;
  std::uniform_real_distribution<double> RandomRealDistributionUnit;
  std::uniform_int_distribution<uint32_t> RandomIntDistribution;
  std::function<double()> RandomDouble;
  std::function<double()> RandomProb;

  uint32_t
  Dimensions = 0,
  SpheresNumber = 0,
  SpheresPerDim = 0,
   * LocCoefs = NULL,
  SavedSteps = 0;

  double
  SphereSize = 0.0,
  StepSize = 0.0,
  VolumeDelta = 0.0,
  Pressure = 0.0,
  Beta = 0.0;

  Point
   * Spheres = NULL,
   * Basis = NULL,
   * SphereCursor = NULL,
   * SystemSize = NULL,
  SystemSizeHalf,
   * * SpheresStored = NULL,
   * * SystemSizeStored = NULL;

  void __LocateSpheres(uint32_t d);
  void __ScaleSpheres(const Point & ratio);
  bool __Overlap(const Point & s1, const Point & s2);
  void __ComputeSystemSize();
  double __ComputePackingFraction();
  bool __MoveParticle();
  bool __ChangeVolume();

public:
  HardSpheres();
  ~HardSpheres();

  Point * GenerateWithBasis(uint32_t dim, Point * basis, uint32_t sph_per_dim, double sph_size);
  Point * GenerateFromFile(char const * filename);

  void UpdateParticles(double part_delta, double vol_delta, uint32_t steps, uint32_t save_step, uint32_t n_part_moves);

  void LoadSpheres(char const * filename);
  void SaveSpheres(char const * filename);
  void SaveStoredSpheres(char const * filename);
};
