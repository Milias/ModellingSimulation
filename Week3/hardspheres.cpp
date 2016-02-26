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
  if (SpheresStored) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] SpheresStored[i];
    }
    delete[] SpheresStored;
  }
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
  //p %= SystemSize;
  return p * p < 4.0 * SphereSize * SphereSize;
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

  if (SpheresPerDim != sph_per_dim || Spheres == NULL) {
    if (Spheres) delete[] Spheres;
    SpheresNumber = pow(sph_per_dim,Dimensions);
    Spheres = new Point[SpheresNumber];
    SphereCursor = Spheres;
    SpheresPerDim = sph_per_dim;
    RandomIntDistribution = std::uniform_int_distribution<uint32_t>(0, SpheresNumber - 1);
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

Point * HardSpheres::GenerateFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  if (Root["FileType"] != "LatticeGenerator") {
    std::cout << "Error: trying to load wrong type of file.\n";
    return NULL;
  }

  Dimensions = Root["Dimensions"].asUInt();
  SpheresNumber = Root["SpheresNumber"].asUInt();
  SphereSize = Root["SphereSize"].asDouble();

  RandomIntDistribution = std::uniform_int_distribution<uint32_t>(0, SpheresNumber - 1);

  if (Spheres) delete[] Spheres;
  Spheres = new Point[SpheresNumber];

  if (SystemSize) delete[] SystemSize;
  SystemSize = new Point[2];

  SystemSize[0].Init(Dimensions);
  SystemSize[1].Init(Dimensions);

  for (uint32_t i = 0; i < Dimensions; i++) {
    SystemSize[0][i] = Root["SystemSize"][0][i].asDouble();
    SystemSize[1][i] = Root["SystemSize"][1][i].asDouble();
  }

  if (Basis) delete[] Basis;
  Basis = new Point[Dimensions];
  for (uint32_t i = 0; i < Dimensions; i++) {
    Basis[i].Init(Dimensions);
    for (uint32_t j = 0; j < Dimensions; j++) {
      Basis[i][j] = Root["Basis"][i][j].asDouble();
    }
  }

  for (uint32_t i = 0; i < SpheresNumber; i++) {
    Spheres[i].Init(Dimensions, 0);
    for (uint32_t j = 0; j < Dimensions; j++) {
      Spheres[i] += Basis[j] * Root["Data"][i][j].asDouble();
    }
  }

  return Spheres;
}

void HardSpheres::UpdateParticles(double delta, uint32_t steps, uint32_t save_step)
{
  if (Spheres == NULL) return;
  if (SpheresStored) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] SpheresStored[i];
    }
    delete[] SpheresStored;
  }
  StepSize = delta;
  SpheresStored = new Point*[steps/save_step];
  for (uint32_t i = 0; i < steps/save_step; i++) {
    SpheresStored[i] = new Point[SpheresNumber];
  }

  SavedSteps = 0;
  Point p(Dimensions), * s;
  bool allowed;
  for (uint32_t i = 0; i < steps; i++) {
    allowed = true;
    // Save spheres
    if (i % save_step == 0) {
      for (uint32_t k = 0; k < SpheresNumber; k++) {
        SpheresStored[SavedSteps][k].Init(Dimensions);
        SpheresStored[SavedSteps][k] = Spheres[k];
      }
      SavedSteps++;
    }

    // Pick random particle.
    s = Spheres + RandomIntDistribution(Generator);
    assert(s - Spheres < SpheresNumber);
    p = *s;

    // Move it.
    for (uint32_t k = 0; k < Dimensions; k++) {
      p += RandomDouble() * StepSize;
    }

    // Check overlap. TODO: quadtree / octree ?
    for (uint32_t k = 0; k < SpheresNumber; k++) {
      if (k == s - Spheres) continue;
      if (__Overlap(p,Spheres[k])) {
        i--;
        allowed = false;
        break;
      }
    }

    // If not overlaping, move it.
    if (allowed) *s = p;
  }
}

void HardSpheres::LoadSpheres(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  if (Root["FileType"] != "InitialCondition") {
    std::cout << "Error: trying to load wrong type of file.\n";
    return;
  }

  Dimensions = Root["Dimensions"].asUInt();
  SpheresNumber = Root["SpheresNumber"].asUInt();
  SphereSize = Root["SphereSize"].asDouble();

  RandomIntDistribution = std::uniform_int_distribution<uint32_t>(0, SpheresNumber - 1);

  if (Spheres) delete[] Spheres;
  Spheres = new Point[SpheresNumber];

  if (SystemSize) delete[] SystemSize;
  SystemSize = new Point[2];

  SystemSize[0].Init(Dimensions);
  SystemSize[1].Init(Dimensions);

  for (uint32_t i = 0; i < Dimensions; i++) {
    SystemSize[0][i] = Root["SystemSize"][0][i].asDouble();
    SystemSize[1][i] = Root["SystemSize"][1][i].asDouble();
  }

  for (uint32_t i = 0; i < SpheresNumber; i++) {
    Spheres[i].Init(Dimensions);
    for (uint32_t j = 0; j < Dimensions; j++) {
      Spheres[i][j] = Root["Data"][j][i].asDouble();
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
  Root["FileType"] = "InitialCondition";

  for (uint32_t i = 0; i < Dimensions; i++) {
    Root["Data"][i] = Json::Value(Json::arrayValue);
  }

  Root["SystemSize"][0] = Json::Value(Json::arrayValue);
  Root["SystemSize"][1] = Json::Value(Json::arrayValue);
  for (uint32_t i = 0; i < Dimensions; i++) {
    Root["SystemSize"][0].append(SystemSize[0][i]);
    Root["SystemSize"][1].append(SystemSize[1][i]);
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

void HardSpheres::SaveStoredSpheres(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = Dimensions;
  Root["SpheresNumber"] = SpheresNumber;
  Root["SpheresPerDim"] = SpheresPerDim;
  Root["SphereSize"] = SphereSize;
  Root["SavedSteps"] = SavedSteps;
  Root["StepSize"] = StepSize;
  Root["FileType"] = "Evolution";

  Root["SystemSize"][0] = Json::Value(Json::arrayValue);
  Root["SystemSize"][1] = Json::Value(Json::arrayValue);
  for (uint32_t i = 0; i < Dimensions; i++) {
    Root["SystemSize"][0].append(SystemSize[0][i]);
    Root["SystemSize"][1].append(SystemSize[1][i]);
  }

  for (uint32_t i = 0; i < Dimensions; i++) {
    Root["Data"][i] = Json::Value(Json::arrayValue);
    for (uint32_t j = 0; j < SavedSteps; j++) {
      Root["Data"][i].append(Json::Value(Json::arrayValue));
    }
  }

  for (uint32_t k = 0; k < SavedSteps; k++) {
    for (uint32_t i = 0; i < SpheresNumber; i++) {
      for (uint32_t j = 0; j < Dimensions; j++) {
        Root["Data"][j][k].append(SpheresStored[k][i][j]);
      }
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
