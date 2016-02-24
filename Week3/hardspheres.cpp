#include "hardspheres.h"

HardSpheres::HardSpheres() {}

HardSpheres::~HardSpheres()
{
  if (Spheres) delete[] Spheres;
  if (Basis) delete[] Basis;
}

void __LocateSpheres(uint32_t d, uint32_t sph)
{
  if (d) {
    for (uint32_t i = 0; i < sph; i++) {
      (*SphereCursor)
    }
  }
}

Point * HardSpheres::GenerateWithBasis(uint32_t dim, Point * basis, uint32_t sph_per_dim)
{
  if (Dimensions != dim || Basis == NULL) {
    if (Basis) delete[] Basis;
    Basis = new Point[dim];
    Dimensions = dim;
  }

  if (SpheresNumber != N || Spheres == NULL) {
    if (Spheres) delete[] Spheres;
    Spheres = new Point[pow(N,Dimensions)];
    SphereCursor = Spheres;
    SpheresNumber = N;
  }

  for (uint32_t i = 0; i < Dimensions; i++) {
    Basis[i] = basis[i];
  }

  for (uint32_t i = 0; i < SpheresNumber; i++) {
    Spheres[i] =
  }
}
