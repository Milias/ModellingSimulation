#pragma once
#include "shared.h"

class Correlation
{
private:
  Point * Spheres = NULL, * SystemSize = NULL, * SystemSizeHalf = NULL;

  double
    * Distances = NULL,
    * BinDistances = NULL,
    SphereSize,
    BinWidth,
    IDistance,
    FDistance,
    * BoxParticleDensity = NULL,
    * NormalizedDensity = NULL;

  uint32_t
    Dimensions,
    SpheresNumber,
    SavedSteps,
    DistancesNumber,
    BinsNumber,
    * DistancesCount = NULL;

  double __Nideal(uint32_t step, uint32_t bin);

  void __InitializeBins();
  void __ComputeDistances(uint32_t step);
  void __CountDistances(uint32_t step);
  void __NormalizeCounts(uint32_t step);

public:
  Correlation();
  ~Correlation();

  /*
    Performs the counting for every step with a fixed number of bins.
  */
  void ComputeHistogram(double r0, double rf, uint32_t bins);

  /*
    Performs the counting for every step with a fixed bin width.
  */
  void ComputeHistogram(double r0, double rf, double dr);

  void LoadSpheres(char const * filename);
  void SaveHistogram(char const * filename);
};
