#include "hardspheres.h"

HardSpheres::HardSpheres()
{
  Generator.seed(DefaultClock::now().time_since_epoch().count());
  RandomRealDistribution = std::uniform_real_distribution<double>(-1.0,1.0);
  RandomDouble = std::bind(RandomRealDistribution, Generator);
}

HardSpheres::~HardSpheres()
{
  if (Spheres) delete[] Spheres;
  if (Basis) delete[] Basis;
}

void HardSpheres::__LocateSpheres(uint32_t d)
{
  if (d) {
    for (; LocCoefs[d] < SpheresPerDim; LocCoefs[d]++) {
      __LocateSpheres(d - 1);
    }
  } else {
    for (; LocCoefs[d] < SpheresPerDim; LocCoefs[d]++) {
      assert(SphereCursor - Spheres < SpheresNumber);
      SphereCursor->Init(Dimensions, 0.0);
      for (uint32_t i = 0; i < Dimensions; i++) {
        (*SphereCursor) += Basis[i] * LocCoefs[i];
      }
      SphereCursor++;
    }
  }
  LocCoefs[d] = 0;
}

bool HardSpheres::__Overlap(const Point & s1, const Point & s2)
{
  Point p(s1);
  p -= s2;
  p %= SystemSize;
  return p * p > 4.0 * SphereSize * SphereSize;
}

Point * HardSpheres::GenerateWithBasis(uint32_t dim, Point * basis, uint32_t sph_per_dim, double sph_size)
{
  if (Dimensions != dim || Basis == NULL || LocCoefs == NULL) {
    if (Basis) delete[] Basis;
    if (LocCoefs) delete[] LocCoefs;
    Basis = new Point[dim];
    LocCoefs = new uint32_t[dim];
    Dimensions = dim;
  }

  if (SpheresPerDim != sph_per_dim ||Spheres == NULL) {
    if (Spheres) delete[] Spheres;
    SpheresNumber = pow(sph_per_dim,Dimensions);
    Spheres = new Point[SpheresNumber];
    SphereCursor = Spheres;
    SpheresPerDim = sph_per_dim;
    RandomIntDistribution = std::uniform_int_distribution<uint32_t>(0, SpheresNumber);
  }

  SphereSize = sph_size;

  for (uint32_t i = 0; i < Dimensions; i++) {
    Basis[i].Init(Dimensions);
    Basis[i] = basis[i];
    LocCoefs[i] = 0;
  }

  __LocateSpheres(Dimensions - 1);
  return Spheres;
}

Point * HardSpheres::GenerateRectangular(uint32_t dim, uint32_t sph_per_dim, double sph_size, double scale)
{
  Point * basis = new Point[dim];

  for (uint32_t i = 0; i < dim; i++) {
    basis[i].Init(dim);
    for (uint32_t j = 0; j < dim; j++) {
      basis[i][j] = i == j ? scale : 0.0;
    }
  }

  Point * p = GenerateWithBasis(dim, basis, sph_per_dim, sph_size);
  delete[] basis;
  return p;
}

Point * HardSpheres::GenerateHexagonal(uint32_t sph_per_dim, double sph_size, double scale)
{
  uint32_t dim = 2;
  Point * basis = new Point[dim];

  for (uint32_t i = 0; i < dim; i++) {
    basis[i].Init(dim);
  }

  basis[0][0] = scale;
  basis[0][1] = 0.0;
  basis[1][0] = 0.5 * scale;
  basis[1][1] = 0.8660254037844386 * scale;

  Point * p = GenerateWithBasis(dim, basis, sph_per_dim, sph_size);
  delete[] basis;
  return p;
}

void HardSpheres::UpdateParticles(double delta, uint32_t steps, uint32_t save_step)
{
  if (Spheres == NULL) return;
  StepSize = delta;

  Point p(Dimensions), * s;
  for (uint32_t i = 0, j = 0; i < steps; i++, j++) {
    // Pick random particle.
    s = Spheres + RandomIntDistribution(Generator);
    p = *s;

    // Move it.
    for (uint32_t k = 0; k < Dimensions; k++) {
      p += RandomDouble() * StepSize;
    }

    // Check overlap. TODO: quadtree / octree.
    for (uint32_t k = 0; k < SpheresNumber; k++) {
      if (__Overlap(p,Spheres[k])) {
        i--; j--;
        break;
      }
    }
  }
}

void HardSpheres::SaveSpheres(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = Dimensions;
  Root["SpheresNumber"] = SpheresNumber;
  Root["SpheresPerDim"] = SpheresPerDim;
  Root["SphereSize"] = SphereSize;

  for (uint32_t i = 0; i < Dimensions; i++) {
    Root["Data"][i] = Json::Value(Json::arrayValue);
  }

  for (uint32_t i = 0; i < SpheresNumber; i++) {
    for (uint32_t j = 0; j < Dimensions; j++) {
      Root["Data"][j].append(Spheres[i][j]);
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
