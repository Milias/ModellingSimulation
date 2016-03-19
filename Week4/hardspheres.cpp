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
  if (SystemSize) delete[] SystemSize;
  if (SpheresStored) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] SpheresStored[i];
    }
    delete[] SpheresStored;
  }
  if (SystemSizeStored) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] SystemSizeStored[i];
    }
    delete[] SystemSizeStored;
  }
  if (StepSizeStored) { delete[] StepSizeStored; }
  if (VolumeDeltaStored) { delete[] VolumeDeltaStored; }
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
  assert(Spheres != nullptr);
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

  // If it's not overlaping, move it.
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

  double delta = RandomDouble() * VolumeDelta;
  for (uint32_t i = 0; i < Dimensions; i++) {
    SystemSize[1][i] += delta;
    if (SystemSize[1][i] < SystemSize[0][i]) {
      delete[] SystemSize;
      SystemSize = old_box;
      return false;
    }
    ratio[i] = (SystemSize[1][i]-SystemSize[0][i])/(old_box[1][i]-old_box[0][i]);

    // Compute volumes.
    old_volume *= (old_box[1][i]-old_box[0][i]);
    new_volume *= (SystemSize[1][i]-SystemSize[0][i]);
  }

  // Compute acceptance.
  double acc = std::exp(-BPSigma*(new_volume - old_volume)/std::pow(2*SphereSize,3) + SpheresNumber*std::log(new_volume/old_volume));

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
    for (uint32_t j = i + 1; j < SpheresNumber; j++) {
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

  delete[] old_box;
  // Everything's OK, update variables and return true.
  SystemSizeHalf = (SystemSize[1] - SystemSize[0]) * 0.5;
  return true;
}

void HardSpheres::__SaveSystem(uint32_t step)
{
  StepSizeStored[step] = StepSize;
  VolumeDeltaStored[step] = VolumeDelta;
  for (uint32_t i = 0; i < SpheresNumber; i++) {
    if (i < 2) SystemSizeStored[step][i] = SystemSize[i];
    SpheresStored[step][i] = Spheres[i];
  }
}

void HardSpheres::__UpdateJsonOutput(Json::Value & root, uint32_t step, StepSizeAdapter & part, StepSizeAdapter & vol)
{
  root["CurrentStep"] = step;
  root["TotalSteps"] = TotalSteps;
  root["PartRate"] = part.rate;
  root["VolRate"] = vol.rate;
  root["PartCount"] = part.count;
  root["VolCount"] = vol.count;
  root["PartDelta"] = part.delta;
  root["VolDelta"] = vol.delta;
}

void HardSpheres::InitializeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  assert(Root["FileType"] == "HardSpheresInit");

  BPSigma = Root["BPSigma"].asDouble();
  ParticleMoves = Root["ParticleMoves"].asUInt();
  VolumeChanges = Root["VolumeChanges"].asUInt();

  StepSize = Root["StepSize"].asDouble();
  VolumeDelta = Root["VolumeDelta"].asDouble();

  TotalSteps = Root["TotalSteps"].asUInt();
  SaveSystemInterval = Root["SaveSystemInterval"].asUInt();
  SavedSteps = SaveSystemInterval > 0 ? TotalSteps / SaveSystemInterval + 1 : 0;

  if (VolumeChanges > 0 && BPSigma == 0) {
    std::cout << "Warning: βPσ^3 set to zero but VolumeChanges > 0, using NVT ensemble.\n";
    VolumeChanges = 0;
  }

  if (VolumeChanges > 0 && VolumeDelta == 0) {
    std::cout << "Warning: VolumeDelta set to zero but VolumeChanges > 0, using NVT ensemble.\n";
    VolumeChanges = 0;
  }

  Initialized = true;
}

void HardSpheres::GenerateLatticeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  assert(Root["FileType"] == "LatticeGenerator");

  Dimensions = Root["Dimensions"].asUInt();
  SpheresNumber = Root["SpheresNumber"].asUInt();
  SphereSize = Root["SphereSize"].asDouble();

  RandomIntDistribution = std::uniform_int_distribution<uint32_t>(0, SpheresNumber - 1);

  if (Spheres) delete[] Spheres;
  Spheres = new Point[SpheresNumber];

  Point * basis = new Point[Dimensions];
  for (uint32_t i = 0; i < Dimensions; i++) {
    basis[i].Init(Dimensions);
    for (uint32_t j = 0; j < Dimensions; j++) {
      basis[i][j] = Root["Basis"][i][j].asDouble();
    }
  }

  for (uint32_t i = 0; i < SpheresNumber; i++) {
    Spheres[i].Init(Dimensions, 0);
    for (uint32_t j = 0; j < Dimensions; j++) {
      Spheres[i] += basis[j] * Root["Data"][i][j].asDouble();
    }
  }

  delete[] basis;

  __ComputeSystemSize();

  SystemSizeHalf.Free(Dimensions);
  SystemSizeHalf = (SystemSize[1] - SystemSize[0]) * 0.5;

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

  if (StepSizeStored) { delete[] StepSizeStored; }
  if (VolumeDeltaStored) { delete[] VolumeDeltaStored; }

  SpheresStored = new Point*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    SpheresStored[i] = new Point[SpheresNumber];
    for (uint32_t j = 0; j < SpheresNumber; j++) {
      SpheresStored[i][j].Init(Dimensions);
    }
  }

  SystemSizeStored = new Point*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    SystemSizeStored[i] = new Point[2];
    for (uint32_t j = 0; j < 2; j++) {
      SystemSizeStored[i][j].Init(Dimensions);
    }
  }

  StepSizeStored = new double[SavedSteps];
  VolumeDeltaStored = new double[SavedSteps];
}

void HardSpheres::UpdateParticles()
{
  assert(Spheres != nullptr && Initialized);

  bool allowed = true;
  StepSizeAdapter part_counter(StepSize, 1e-6 * SphereSize, 2.0 * SphereSize);
  StepSizeAdapter vol_counter(VolumeDelta, 1e-6 * SphereSize, 10.0 * SphereSize);

  Json::Value Progress;
  Json::FastWriter writer;
  __UpdateJsonOutput(Progress, 0, part_counter, vol_counter);
  std::cout << writer.write(Progress);

  uint32_t step = 0;
  for (uint32_t i = 0; i < TotalSteps; i++) {
    // Save spheres.
    if ((SaveSystemInterval > 0 ? i % SaveSystemInterval == 0 : false)) {
      __SaveSystem(step);
      step++;
    }

    if (i % 100 == 0) {
      __UpdateJsonOutput(Progress, i, part_counter, vol_counter);
      std::cout << writer.write(Progress);
    }

    // Move one particle.
    for (uint32_t j = 0; j < ParticleMoves; j++) {
      allowed = __MoveParticle();
      StepSize = part_counter.Update(allowed);
    }

    // Change volume.
    for (uint32_t j = 0; j < VolumeChanges; j++) {
      //allowed =
      __ChangeVolume();
      // Do not modify volume delta change yet.
      //VolumeDelta = vol_counter.Update(allowed);
    }
  }
  if (SavedSteps > 0) {
    __SaveSystem(SavedSteps-1);
    SavedSteps = step+1;
  }
  __UpdateJsonOutput(Progress, TotalSteps, part_counter, vol_counter);
  std::cout << writer.write(Progress);
}

void HardSpheres::LoadSpheres(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  //assert(Root["FileType"] == "LatticeGenerator");

  Dimensions = Root["Dimensions"].asUInt();
  SpheresNumber = Root["SpheresNumber"].asUInt();
  SphereSize = Root["SphereSize"].asDouble();

  RandomIntDistribution = std::uniform_int_distribution<uint32_t>(0, SpheresNumber - 1);
  uint32_t starting_step = Root["SavedSteps"].asUInt() - 1;

  if (Spheres) delete[] Spheres;
  Spheres = new Point[SpheresNumber];

  if (SystemSize) delete[] SystemSize;
  SystemSize = new Point[2];

  SystemSize[0].Init(Dimensions);
  SystemSize[1].Init(Dimensions);

  for (uint32_t i = 0; i < Dimensions; i++) {
    SystemSize[0][i] = Root["SystemSize"][starting_step][0][i].asDouble();
    SystemSize[1][i] = Root["SystemSize"][starting_step][1][i].asDouble();
  }

  SystemSizeHalf.Free(Dimensions);
  SystemSizeHalf = (SystemSize[1] - SystemSize[0]) * 0.5;

  for (uint32_t i = 0; i < SpheresNumber; i++) {
    Spheres[i].Init(Dimensions);
    for (uint32_t j = 0; j < Dimensions; j++) {
      Spheres[i][j] = Root["Data"][starting_step][i][j].asDouble();
    }
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

  if (StepSizeStored) { delete[] StepSizeStored; }
  if (VolumeDeltaStored) { delete[] VolumeDeltaStored; }

  SpheresStored = new Point*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    SpheresStored[i] = new Point[SpheresNumber];
    for (uint32_t j = 0; j < SpheresNumber; j++) {
      SpheresStored[i][j].Init(Dimensions);
    }
  }

  SystemSizeStored = new Point*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    SystemSizeStored[i] = new Point[2];
    for (uint32_t j = 0; j < 2; j++) {
      SystemSizeStored[i][j].Init(Dimensions);
    }
  }

  StepSizeStored = new double[SavedSteps];
  VolumeDeltaStored = new double[SavedSteps];
}

void HardSpheres::SaveSpheres(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = Dimensions;
  Root["SpheresNumber"] = SpheresNumber;
  Root["SphereSize"] = SphereSize;
  Root["SavedSteps"] = SavedSteps;
  Root["FileType"] = "SystemEvolution";
  Root["BPSigma"] = BPSigma;
  Root["ParticleMoves"] = ParticleMoves;
  Root["VolumeChanges"] = VolumeChanges;

  Root["TotalSteps"] = TotalSteps;
  Root["SaveSystemInterval"] = SaveSystemInterval;

  for (uint32_t i = 0; i < SavedSteps; i++) {
    Root["SystemSize"][i] = Json::Value(Json::arrayValue);
    Root["Data"][i] = Json::Value(Json::arrayValue);
    Root["StepSize"][i] = StepSizeStored[i];
    Root["VolumeDelta"][i] = VolumeDeltaStored[i];
    for (uint32_t j = 0; j < SpheresNumber; j++) {
      if (j < 2) Root["SystemSize"][i].append(Json::arrayValue);
      Root["Data"][i].append(Json::arrayValue);
    }
  }

  for (uint32_t i = 0; i < SavedSteps; i++) {
    for (uint32_t j = 0; j < SpheresNumber; j++) {
      for (uint32_t k = 0; k < Dimensions; k++) {
        if (j < 2) Root["SystemSize"][i][j].append(SystemSizeStored[i][j][k]);
        Root["Data"][i][j].append(SpheresStored[i][j][k]);
      }
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
