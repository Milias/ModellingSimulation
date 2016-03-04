#include "hardspheres.h"

HardSpheres::HardSpheres()
{
  Generator.seed(DefaultClock::now().time_since_epoch().count());
  RandomRealDistribution = std::uniform_real_distribution<double>(-1.0,1.0);
  RandomRealDistributionUnit = std::uniform_real_distribution<double>(0.0,1.0);
  RandomDouble = std::bind(RandomRealDistribution, Generator);
  RandomProb = std::bind(RandomRealDistributionUnit, Generator);
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

void HardSpheres::__ScaleSpheres(const Point & ratio)
{
  for (uint32_t i = 0; i < SpheresNumber; i++) {
    for (uint32_t j = 0; j < Dimensions; j++) {
      Spheres[i][j] = (Spheres[i][j] - SystemSize[0][j])*ratio[j] + SystemSize[0][j];
    }
  }
}

bool HardSpheres::__Overlap(const Point & s1, const Point & s2)
{
  Point p(s2);
  p -= s1;
  p %= SystemSizeHalf;
  return p * p < 4.0 * SphereSize * SphereSize;
}

void HardSpheres::__ComputeSystemSize()
{
  if (Spheres == NULL) return;

  if (SystemSize) delete[] SystemSize;
  SystemSize = new Point[2];

  SystemSize[0].Init(Dimensions);
  SystemSize[1].Init(Dimensions);

  SystemSize[0] = SystemSize[1] = Spheres[0];

  for (uint32_t i = 1; i < SpheresNumber; i++) {
    for (uint32_t j = 0; j < Dimensions; j++) {
      if (Spheres[i][j] > SystemSize[1][j]) {
        SystemSize[1][j] = Spheres[i][j];
      } else if (Spheres[i][j] < SystemSize[0][j]) {
        SystemSize[0][j] = Spheres[i][j];
      }
    }
  }

  for (uint32_t i = 0; i < Dimensions; i++) {
    SystemSize[0][i] -= SphereSize*1.1;
    SystemSize[1][i] += SphereSize*1.1;
  }
}

double HardSpheres::__ComputePackingFraction()
{
  double vol_cube = 1.0;
  for (uint32_t i = 0; i < Dimensions; i++) {
    vol_cube *= SystemSize[1][i]-SystemSize[0][i];
  }

  double vol_sphere = Dimensions == 3 ? 4*pi/3*std::pow(SphereSize,3) : pi*std::pow(SphereSize,2);
  return SpheresNumber*vol_sphere/vol_cube;
}

bool HardSpheres::__MoveParticle()
{
  // Pick random particle.
  Point * s = Spheres + RandomIntDistribution(Generator);
  assert(s - Spheres < SpheresNumber);
  Point p(*s);

  // Move it.
  for (uint32_t k = 0; k < Dimensions; k++) {
    p[k] += RandomDouble() * StepSize;
  }
  p.Wrap(SystemSize[0], SystemSize[1]);

  // Check overlap. TODO: quadtree / octree ?
  for (uint32_t k = 0; k < SpheresNumber; k++) {
    if (k == s - Spheres) { continue; }
    if (__Overlap(p,Spheres[k])) {
      return false;
    }
  }

  // If not overlaping, move it.
  *s = p;
  return true;
}

bool HardSpheres::__ChangeVolume()
{
  Point ratio(Dimensions);
  Point * old_box = SystemSize;
  double old_volume = 1.0, new_volume = 1.0;

  // Copy old box size to new variable that we will modify.
  SystemSize = new Point[2];
  for (uint32_t i = 0; i < 2; i++) {
    SystemSize[i].Init(Dimensions);
    SystemSize[i] = old_box[i];
  }

  for (uint32_t i = 0; i < Dimensions; i++) {
    SystemSize[1][i] += RandomDouble() * VolumeDelta;
    if (SystemSize[1][i] < SystemSize[0][i]) {
      // If it's been rejected, delete the proposed one and
      // recover the old one.
      delete[] SystemSize;
      SystemSize = old_box;
      return false;
    }
    ratio[i] = (SystemSize[1][i]-SystemSize[0][i])/(old_box[1][i]-old_box[0][i]);

    // Compute volumes.
    old_volume *= old_box[1][i]-old_box[0][i];
    new_volume *= SystemSize[1][i]-SystemSize[0][i];
  }

  // Compute acceptance.
  double acc = std::exp(-(BPSigma*(new_volume - old_volume) - SpheresNumber*std::log(new_volume/old_volume)));

  // If rejected, recover everything, otherwise proceed
  // with overlap checking.
  if (RandomProb() > acc) {
    delete[] SystemSize;
    SystemSize = old_box;
    return false;
  }

  // Scaling new spheres.
  __ScaleSpheres(ratio);

  // Checking overlap. TODO: there should be possible to do this more efficiently...
  for (uint32_t i = 0; i < SpheresNumber; i++) {
    for (uint32_t j = 0; j < SpheresNumber; j++) {
      if (i==j) continue;
      if (__Overlap(Spheres[i],Spheres[j])) {
        // Reject everything.
        for (uint32_t k = 0; k < Dimensions; k++) {
          ratio[k] = 1.0/ratio[k];
        }
        __ScaleSpheres(ratio);
        delete[] SystemSize;
        SystemSize = old_box;
        return false;
      }
    }
  }

  // Everything's OK, return true.
  return true;
}

void HardSpheres::__SaveSystem(uint32_t step)
{
  for (uint32_t k = 0; k < SpheresNumber; k++) {
    SpheresStored[step][k].Init(Dimensions);
    SpheresStored[step][k] = Spheres[k];
  }
  for (uint32_t k = 0; k < 2; k++) {
    SystemSizeStored[step][k].Init(Dimensions);
    SystemSizeStored[step][k] = SystemSize[k];
  }
}

Point * HardSpheres::GenerateLatticeWithBasis(uint32_t dim, Point * basis, uint32_t sph_per_dim, double sph_size)
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

Point * HardSpheres::GenerateLatticeFromFile(char const * filename)
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

  __ComputeSystemSize();

  SystemSizeHalf.Free();
  SystemSizeHalf.Init(Dimensions);
  SystemSizeHalf = (SystemSize[1] - SystemSize[0]) * 0.5;

  return Spheres;
}

void HardSpheres::InitializeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  if (Root["FileType"] != "HardSpheresInit") {
    std::cout << "Error: wrong file type. Nothing done.\n";
    return;
  }

  BPSigma = Root["BPSigma"].asDouble();
  ParticleMoves = Root["ParticleMoves"].asUInt();
  VolumeChanges = Root["VolumeChanges"].asUInt();

  StepSize = Root["StepSize"].asDouble();
  VolumeDelta = Root["VolumeDelta"].asDouble();

  TotalSteps = Root["TotalSteps"].asUInt();
  SaveSystemInterval = Root["SaveSystemInterval"].asUInt();
  SavedSteps = TotalSteps / SaveSystemInterval + 1;

  if (VolumeChanges > 0 && BPSigma == 0) {
    std::cout << "Warning: βPσ^3 set to zero but VolumeChanges > 0, using NVT ensemble.\n";
    VolumeChanges = 0;
  }

  if (SpheresStored) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] SpheresStored[i];
    }
    delete[] SpheresStored;
  }
  if (SystemSizeStored) {
    for (uint32_t i = 0; i < 2; i++) {
      delete[] SystemSizeStored[i];
    }
    delete[] SystemSizeStored;
  }

  SpheresStored = new Point*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    SpheresStored[i] = new Point[SpheresNumber];
  }
  SystemSizeStored = new Point*[SavedSteps];
  for (uint32_t i = 0; i < 2; i++) {
    SystemSizeStored[i] = new Point[SpheresNumber];
  }

  Initialized = true;
}

void HardSpheres::UpdateParticles()
{
  if (Spheres == NULL || !Initialized) return;

  bool allowed = true;
  StepSizeAdapter part_counter(StepSize, 0.001 * SphereSize, 2.0 * SphereSize);
  StepSizeAdapter vol_counter(VolumeDelta, 0.001 * SphereSize, 2.0 * SphereSize);

  for (uint32_t i = 0, step = 0; i < TotalSteps; i++) {
    // Save spheres
    if ((SaveSystemInterval > 0 ? i % SaveSystemInterval == 0 : false) && (step < SavedSteps) && allowed) {
      __SaveSystem(step);
      step++;
    }

    // Move one particle.
    for (uint32_t j = 0; j < ParticleMoves; j += (allowed ? 1 : 0)) {
      allowed = __MoveParticle();
      StepSize = part_counter.Update(allowed);
    }

    // Change volume.
    for (uint32_t j = 0; j < VolumeChanges; j += (allowed ? 1 : 0)) {
      allowed = __ChangeVolume();
      VolumeDelta = vol_counter.Update(allowed);
    }
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

  for (uint32_t i = 0; i < SpheresNumber; i++) {
    Spheres[i].Init(Dimensions);
    for (uint32_t j = 0; j < Dimensions; j++) {
      Spheres[i][j] = Root["Data"][j][i].asDouble();
    }
  }

  __ComputeSystemSize();

  SystemSizeHalf.Free();
  SystemSizeHalf.Init(Dimensions);
  SystemSizeHalf = (SystemSize[1] - SystemSize[0]) * 0.5;
}

void HardSpheres::SaveSpheres(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = Dimensions;
  Root["SpheresNumber"] = SpheresNumber;
  Root["SphereSize"] = SphereSize;
  Root["PackFraction"] = __ComputePackingFraction();
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
  Root["SphereSize"] = SphereSize;
  Root["SavedSteps"] = SavedSteps;
  Root["PackFraction"] = __ComputePackingFraction();
  Root["FileType"] = "Evolution";

  for (uint32_t i = 0; i < 2; i++) {
    Root["SystemSize"][i] = Json::Value(Json::arrayValue);
    for (uint32_t j = 0; j < SavedSteps; j++) {
      Root["SystemSize"][i].append(Json::arrayValue);
    }
  }

  for (uint32_t k = 0; k < SavedSteps; k++) {
    for (uint32_t i = 0; i < 2; i++) {
      for (uint32_t j = 0; j < Dimensions; j++) {
        Root["SystemSize"][j][k].append(SpheresStored[k][i][j]);
      }
    }
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
