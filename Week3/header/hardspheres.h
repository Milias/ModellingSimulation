#pragma once
#include "shared.h"

class HardSpheres
{
private:
  uint32_t Dimensions = 0, SpheresNumber = 0;
  double SphereSize = 0.0;
  Point * Spheres = NULL, * Basis = NULL, * SphereCursor = NULL;

  void __LocateSpheres(uint32_t d, uint32_t sph);

public:
  HardSpheres();
  ~HardSpheres();

  Point * GenerateWithBasis(uint32_t dim, Point * basis, uint32_t N);
};
