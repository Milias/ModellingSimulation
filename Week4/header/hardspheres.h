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

  bool Initialized = false;

  uint32_t
    Dimensions = 0,
    SpheresNumber = 0,
    ParticleMoves = 0,
    VolumeChanges = 0,
    TotalSteps = 0,
    SaveSystemInterval = 0,
    SavedSteps = 0;

  double
    SphereSize = 0.0,
    StepSize = 0.0,
    VolumeDelta = 0.0,
    BPSigma = 0.0,
     * StepSizeStored = NULL,
     * VolumeDeltaStored = NULL;

  Point
     * Spheres = NULL,
     * SystemSize = NULL,
    SystemSizeHalf,
     * * SpheresStored = NULL,
     * * SystemSizeStored = NULL;

  void __ScaleSpheres(const Point & ratio);
  bool __Overlap(const Point & s1, const Point & s2);
  void __ComputeSystemSize();
  bool __MoveParticle();
  bool __ChangeVolume();

  void __SaveSystem(uint32_t step);
  void __UpdateJsonOutput(Json::Value & root, uint32_t step, StepSizeAdapter & part, StepSizeAdapter & vol);

public:
  HardSpheres();
  ~HardSpheres();

  void InitializeFromFile(char const * filename);
  void GenerateLatticeFromFile(char const * filename);
  void UpdateParticles();

  void LoadSpheres(char const * filename);
  void SaveSpheres(char const * filename);
};
