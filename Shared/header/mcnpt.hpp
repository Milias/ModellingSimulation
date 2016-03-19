#pragma once
#include "shared.h"
#include "vector.hpp"
#include "part.hpp"

template <uint32_t D, class Particle> class MonteCarloSimulatorNPT
{
protected:
  uint32_t
    ParticlesNumber = 0,
    ParticleMoves = 0,
    VolumeChanges = 0,
    TotalSteps = 0,
    SaveSystemInterval = 0,
    SavedSteps = 0,
    DistancesNumber = 0;

  double
    ParticleRadius = 0.0,
    StepSize = 0.0,
    VolumeDelta = 0.0,
    BPSigma = 0.0,
    * StoredStepSize = nullptr,
    * StoredVolumeDelta = nullptr,
    * Distances = nullptr;

  Particle
    * Particles = nullptr,
    ** StoredParticles = nullptr;

  Vector<D>
    * SystemSize = nullptr,
    HalfSystemSize,
    ** StoredSystemSize = nullptr;

  RandomGenerator Random;

  void __ScaleParticles(const Vector<D> & ratio);
  bool __Overlap(const Particle & p1, const Particle & p2, double dist);
  double __ComputeDistance2(const Vector<D> & p1, const Vector<D> & p2);
  double __ComputeDistance(const Vector<D> & p1, const Vector<D> & p2);
  void __ComputeSystemSize();
  void __SaveSystem(uint32_t step);

  virtual bool __MoveParticle();
  virtual bool __ChangeVolume();
  virtual inline void __Measure() {}

  virtual void __UpdateJsonOutput(Json::Value & root, uint32_t step, StepSizeAdapter & part, StepSizeAdapter & vol);

public:
  MonteCarloSimulatorNPT();
  ~MonteCarloSimulatorNPT();

  virtual void InitializeFromFile(char const * filename);
  void UpdateParticles();

  virtual void LoadParticles(char const * filename);
  virtual void PostLoadParticles(Json::Value & root) {}
  virtual void SaveParticles(char const * filename);
};

template <uint32_t D, class Particle> MonteCarloSimulatorNPT<D, Particle>::MonteCarloSimulatorNPT() {}

template <uint32_t D, class Particle> MonteCarloSimulatorNPT<D, Particle>::~MonteCarloSimulatorNPT()
{
  if (Particles) delete[] Particles;
  if (SystemSize) delete[] SystemSize;
  if (StoredStepSize) { delete[] StoredStepSize; }
  if (StoredVolumeDelta) { delete[] StoredVolumeDelta; }

  if (StoredParticles) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] StoredParticles[i];
    }
    delete[] StoredParticles;
  }

  if (StoredSystemSize) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] StoredSystemSize[i];
    }
    delete[] StoredSystemSize;
  }
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::__ScaleParticles(const Vector<D> & ratio)
{
  for (uint32_t i = 0; i < ParticlesNumber; i++) {
    for (uint32_t j = 0; j < D; j++) {
      Particles[i].X[j] = (Particles[i].X[j] - SystemSize[0][j])*ratio[j] + SystemSize[0][j];
    }
  }
}

template <uint32_t D, class Particle> bool MonteCarloSimulatorNPT<D, Particle>::__Overlap(const Particle & p1, const Particle & p2, double dist)
{
  return __ComputeDistance2(p1.X, p2.X) < dist * dist;
}

template <uint32_t D, class Particle> double MonteCarloSimulatorNPT<D, Particle>::__ComputeDistance2(const Vector<D> & p1, const Vector<D> & p2)
{
  Vector<D> p(p1);
  p -= p2;
  p %= HalfSystemSize;
  return p * p;
}

template <uint32_t D, class Particle> double MonteCarloSimulatorNPT<D, Particle>::__ComputeDistance(const Vector<D> & p1, const Vector<D> & p2)
{
  return std::sqrt(__ComputeDistance2(p1,p2));
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::__ComputeSystemSize()
{
  if (SystemSize) delete[] SystemSize;
  SystemSize = new Point[2];

  SystemSize[0].Init(D);
  SystemSize[1].Init(D);

  SystemSize[0] = SystemSize[1] = Particles->X;

  for (uint32_t i = 1; i < ParticlesNumber; i++) {
    for (uint32_t j = 0; j < D; j++) {
      if (Particles[i].X[j] > SystemSize[1][j]) {
        SystemSize[1][j] = Particles[i].X[j];
      } else if (Particles[i].X[j] < SystemSize[0][j]) {
        SystemSize[0][j] = Particles[i].X[j];
      }
    }
  }

  for (uint32_t i = 0; i < D; i++) {
    SystemSize[0][i] -= 0.001;
    SystemSize[1][i] += 0.001;
  }
}

template <uint32_t D, class Particle> bool MonteCarloSimulatorNPT<D, Particle>::__MoveParticle()
{
  // Pick random particle and copy it.
  Particle * old_pos = Particles + Random.RandomInt();
  Particle new_pos(old_pos);

  // Move it.
  for (uint32_t k = 0; k < D; k++) {
    new_pos.X[k] += Random.RandomDouble() * StepSize;
  }
  new_pos.X.Wrap(SystemSize[0], SystemSize[1]);

  // Check overlap. TODO: quadtree / octree ?
  for (uint32_t k = 0; k < ParticlesNumber; k++) {
    if (k == old_pos - Particles) { continue; }
    if (__Overlap(new_pos, Particles[k], 2.0 * ParticleRadius)) {
      return false;
    }
  }

  // If it's not overlaping, move it.
  *old_pos = new_pos;
  return true;
}

template <uint32_t D, class Particle> bool MonteCarloSimulatorNPT<D, Particle>::__ChangeVolume()
{
  Vector<D> ratio(0.0);
  Vector<D> * old_box = SystemSize;
  double old_volume = 1.0, new_volume = 1.0;

  // Copy old box size to new variable that we will modify.
  SystemSize = new Vector<D>[2];
  for (uint32_t i = 0; i < 2; i++) {
    SystemSize[i] = old_box[i];
  }

  double delta = Random.RandomDouble() * VolumeDelta;
  for (uint32_t i = 0; i < D; i++) {
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
  double acc = std::exp(-BPSigma*(new_volume - old_volume) + ParticlesNumber*std::log(new_volume/old_volume));

  // If rejected, recover everything, otherwise proceed
  // with overlap checking.
  if (Random.RandomProb() > acc) {
    delete[] SystemSize;
    SystemSize = old_box;
    return false;
  }

  // Scaling new spheres.
  __ScaleParticles(ratio);

  // Checking overlap. TODO: there should be possible to do this more efficiently...
  for (uint32_t i = 0; i < ParticlesNumber; i++) {
    for (uint32_t j = i + 1; j < ParticlesNumber; j++) {
      if (__Overlap(Particles[i], Particles[j], 2.0 * ParticleRadius)) {
        // Reject everything.
        __ScaleParticles(1.0/ratio);
        delete[] SystemSize;
        SystemSize = old_box;
        return false;
      }
    }
  }

  delete[] old_box;
  // Everything's OK, update variables and return true.
  HalfSystemSize = SystemSize[1] - SystemSize[0];
  HalfSystemSize *= 0.5;
  return true;
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::__SaveSystem(uint32_t step)
{
  StoredStepSize[step] = StepSize;
  StoredVolumeDelta[step] = VolumeDelta;
  for (uint32_t i = 0; i < ParticlesNumber; i++) {
    if (i < 2) StoredSystemSize[step][i] = SystemSize[i];
    StoredParticles[step][i] = Particles[i];
  }
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::__UpdateJsonOutput(Json::Value & root, uint32_t step, StepSizeAdapter & part, StepSizeAdapter & vol)
{
  root["CurrentStep"] = step;
  root["TotalSteps"] = TotalSteps;
  root["PartRate"] = part.rate;
  root["VolRate"] = vol.rate;
  root["PartCount"] = part.count;
  root["VolCount"] = vol.count;
  root["PartDelta"] = part.delta;
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::InitializeFromFile(char const * filename)
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
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::LoadParticles(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  assert(Root["Dimensions"].asUInt() == D);

  ParticlesNumber = Root["ParticlesNumber"].asUInt();
  ParticleRadius = Root["ParticleRadius"].asDouble();

  if (ParticleMoves == 0) ParticleMoves = ParticlesNumber;

  Random.SetRange(0, ParticlesNumber - 1);
  uint32_t starting_step = Root["SavedSteps"].asUInt() - 1;

  if (Particles) delete[] Particles;
  Particles = new Particle[ParticlesNumber];

  if (SystemSize) delete[] SystemSize;
  SystemSize = new Vector<D>[2];

  for (uint32_t i = 0; i < D; i++) {
    SystemSize[0][i] = Root["SystemSize"][starting_step][0][i].asDouble();
    SystemSize[1][i] = Root["SystemSize"][starting_step][1][i].asDouble();
  }

  HalfSystemSize = SystemSize[1] - SystemSize[0];
  HalfSystemSize *= 0.5;

  for (uint32_t i = 0; i < ParticlesNumber; i++) {
    for (uint32_t j = 0; j < D; j++) {
      Particles[i].X[j] = Root["Particles"][starting_step][i][j].asDouble();
    }
  }

  if (StoredParticles) {
    for (uint32_t i = 0; i < SavedSteps; i++) { delete[] StoredParticles[i]; }
    delete[] StoredParticles;
  }

  if (StoredSystemSize) {
    for (uint32_t i = 0; i < 2; i++) { delete[] StoredSystemSize[i]; }
    delete[] StoredSystemSize;
  }

  if (StoredStepSize) { delete[] StoredStepSize; }
  if (StoredVolumeDelta) { delete[] StoredVolumeDelta; }

  StoredParticles = new Particle*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    StoredParticles[i] = new Particle[ParticlesNumber];
  }

  StoredSystemSize = new Vector<D>*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    StoredSystemSize[i] = new Vector<D>[2];
  }

  StoredStepSize = new double[SavedSteps];
  StoredVolumeDelta = new double[SavedSteps];

  PostLoadParticles(Root);
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::UpdateParticles()
{
  StepSizeAdapter part_counter(StepSize, 1e-6 * ParticleRadius, 2.0 * ParticleRadius);
  StepSizeAdapter vol_counter(VolumeDelta, 0.0, 0.0);

  Json::Value Progress;
  Json::FastWriter writer;
  __UpdateJsonOutput(Progress, 0, part_counter, vol_counter);
  std::cout << writer.write(Progress);

  uint32_t step = 0, print_steps = std::min(uint32_t(100),TotalSteps/100+1);
  for (uint32_t i = 0; i < TotalSteps; i++) {
    // Save spheres.
    if (SaveSystemInterval > 0 && i % SaveSystemInterval == 0) {
      __SaveSystem(step);
      step++;
    }

    if (i % print_steps == 0) {
      __UpdateJsonOutput(Progress, i, part_counter, vol_counter);
      std::cout << writer.write(Progress);
    }

    // Try to move one particle.
    for (uint32_t j = 0; j < ParticleMoves; j++) {
      StepSize = part_counter.Update(__MoveParticle());
    }

    // Try to change volume.
    for (uint32_t j = 0; j < VolumeChanges; j++) {
      vol_counter.Update(__ChangeVolume());
    }

    __Measure();
  }

  if (SavedSteps > 0) {
    __SaveSystem(step);
    SavedSteps = step+1;
  }
  __UpdateJsonOutput(Progress, TotalSteps, part_counter, vol_counter);
  std::cout << writer.write(Progress);
}

template <uint32_t D, class Particle> void MonteCarloSimulatorNPT<D, Particle>::SaveParticles(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = D;
  Root["ParticlesNumber"] = ParticlesNumber;
  Root["SavedSteps"] = SavedSteps;
  Root["FileType"] = "SystemEvolution";
  Root["BPSigma"] = BPSigma;
  Root["ParticleMoves"] = ParticleMoves;
  Root["VolumeChanges"] = VolumeChanges;
  Root["ParticlesRadius"] = ParticleRadius;

  Root["TotalSteps"] = TotalSteps;
  Root["SaveSystemInterval"] = SaveSystemInterval;

  for (uint32_t i = 0; i < SavedSteps; i++) {
    for (uint32_t j = 0; j < ParticlesNumber; j++) {
      for (uint32_t k = 0; k < D; k++) {
        if (j < 2) Root["SystemSize"][i][j][k] = StoredSystemSize[i][j][k];
        Root["Particles"][i][j][k] = StoredParticles[i][j].X[k];
      }
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
