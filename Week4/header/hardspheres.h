#pragma once
#include "shared.h"

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
    if (rate < 0.4) {
      delta = std::fmax(delta * 0.9, min);
    } else if (rate > 0.6) {
      delta = std::fmin(delta * 1.1, max);
    }
    return delta;
  }
};

class HardSpheres
{
private:
  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealDistribution;
  std::uniform_real_distribution<double> RandomRealDistributionUnit;
  std::uniform_int_distribution<uint32_t> RandomIntDistribution;
  std::function<double()> RandomDouble;
  std::function<double()> RandomProb;

  bool Initialized = false;

  uint32_t
    Dimensions = 0,
    SpheresNumber = 0,
    SpheresPerDim = 0,
     * LocCoefs = NULL,
    ParticleMoves = 0,
    VolumeChanges = 0,
    TotalSteps = 0,
    SaveSystemInterval = 0
    SavedSteps = 0,;

  double
    SphereSize = 0.0,
    StepSize = 0.0,
    VolumeDelta = 0.0,
    BPSigma = 0.0;

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

  void __SaveSystem(uint32_t step);

public:
  HardSpheres();
  ~HardSpheres();

  Point * GenerateLatticeWithBasis(uint32_t dim, Point * basis, uint32_t sph_per_dim, double sph_size);
  Point * GenerateLatticeFromFile(char const * filename);
  void InitializeFromFile(char const * filename);

  void UpdateParticles(double part_delta, double vol_delta, uint32_t steps, uint32_t save_step, uint32_t n_part_moves);

  void LoadSpheres(char const * filename);
  void SaveSpheres(char const * filename);
  void SaveStoredSpheres(char const * filename);
};
