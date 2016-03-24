#pragma once
#include "shared.h"
#include "vector.hpp"
#include "part.hpp"

template <uint32_t D, class Particle> class MonteCarloSimulatorMuVT
{
protected:
  uint32_t
    MaxParticlesNumber = 0,
    ParticlesNumber = 0,
    ParticleMoves = 0,
    TotalSteps = 0,
    SaveSystemInterval = 0,
    SavedSteps = 0,
    * StoredParticlesNumber = nullptr;

  double
    ParticlesRadius = 0.0,
    StepSize = 0.0,
    Volume = 0.0,
    Density = 0.0,
    Beta = 0.0,
    Mu = 0.0,
    * StoredStepSize = nullptr,
    * StoredDensity = nullptr;

  Particle
    * Particles = nullptr,
    ** StoredParticles = nullptr;

  Vector<D>
    * SystemSize = nullptr,
    HalfSystemSize;

  RandomGenerator Random;

  virtual void __PostInitialize(Json::Value & root) {}

  double __ComputeDistance2(const Vector<D> & p1, const Vector<D> & p2);
  double __ComputeDistance(const Vector<D> & p1, const Vector<D> & p2);
  void __SaveSystem(uint32_t step);
  virtual void __PostSaveSystem(uint32_t step) {}

  virtual double __ParticleEnergy(Particle * p) = 0;
  virtual bool __MoveParticle() = 0;
  virtual bool __AddParticle() = 0;
  virtual bool __RemoveParticle() = 0;
  virtual void __Measure() {}

  virtual void __UpdateJsonOutput(Json::Value & root, uint32_t step, StepSizeAdapter & part);
  virtual void __PostLoadParticles(Json::Value & root) {}
  virtual void __PostSaveParticles(Json::Value & root) {}

public:
  MonteCarloSimulatorMuVT() {}
  ~MonteCarloSimulatorMuVT();

  virtual void InitializeFromFile(char const * filename);

  void UpdateParticles();
  void LoadParticles(char const * filename);
  void SaveParticles(char const * filename);
};

template <uint32_t D, class Particle> MonteCarloSimulatorMuVT<D, Particle>::~MonteCarloSimulatorMuVT()
{
  if (Particles) delete[] Particles;
  if (SystemSize) delete[] SystemSize;
  if (StoredStepSize) { delete[] StoredStepSize; }
  if (StoredParticlesNumber) { delete[] StoredParticlesNumber; }

  if (StoredParticles) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      delete[] StoredParticles[i];
    }
    delete[] StoredParticles;
  }
}

template <uint32_t D, class Particle> double MonteCarloSimulatorMuVT<D, Particle>::__ComputeDistance2(const Vector<D> & p1, const Vector<D> & p2)
{
  Vector<D> p(p1);
  p -= p2;
  p %= HalfSystemSize;
  return p * p;
}

template <uint32_t D, class Particle> double MonteCarloSimulatorMuVT<D, Particle>::__ComputeDistance(const Vector<D> & p1, const Vector<D> & p2)
{
  return std::sqrt(__ComputeDistance2(p1,p2));
}

template <uint32_t D, class Particle> void MonteCarloSimulatorMuVT<D, Particle>::__SaveSystem(uint32_t step)
{
  StoredStepSize[step] = StepSize;
  for (uint32_t i = 0; i < ParticlesNumber; i++) {
    StoredParticles[step][i] = Particles[i];
  }

  __PostSaveSystem(step);
}

template <uint32_t D, class Particle> void MonteCarloSimulatorMuVT<D, Particle>::__UpdateJsonOutput(Json::Value & root, uint32_t step, StepSizeAdapter & part)
{
  root["CurrentStep"] = step;
  root["TotalSteps"] = TotalSteps;
  root["PartRate"] = part.rate;
  root["PartCount"] = part.count;
  root["PartDelta"] = part.delta;
}

template <uint32_t D, class Particle> void MonteCarloSimulatorMuVT<D, Particle>::InitializeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  ParticleMoves = Root["ParticleMoves"].asUInt();
  StepSize = Root["StepSize"].asDouble();
  Beta = Root["Beta"].asDouble();

  TotalSteps = Root["TotalSteps"].asUInt();
  SaveSystemInterval = Root["SaveSystemInterval"].asUInt();
  SavedSteps = SaveSystemInterval > 0 ? TotalSteps / SaveSystemInterval + 1 : 0;

  __PostInitialize(Root);
}

template <uint32_t D, class Particle> void MonteCarloSimulatorMuVT<D, Particle>::LoadParticles(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  if (Root["Dimensions"].asUInt() != D) {
    std::cout << "Error: wrong number of dimensions. Should be " << D << " (" << Root["Dimensions"].asUInt() << ")" << std::endl;
    return;
  }

  ParticlesNumber = Root["ParticlesNumber"].asUInt();
  ParticlesRadius = Root["ParticlesRadius"].asDouble();
  Mu = Root["Mu"].asDouble();

  if (ParticleMoves == 0) ParticleMoves = ParticlesNumber;

  Random.SetRange(0, ParticlesNumber - 1);
  uint32_t starting_step = Root["SavedSteps"].asUInt() - 1;

  if (Particles) delete[] Particles;
  Particles = new Particle[ParticlesNumber];

  if (SystemSize) delete[] SystemSize;
  SystemSize = new Vector<D>[2];

  for (uint32_t i = 0; i < D; i++) {
    SystemSize[0][i] = Root["SystemSize"][0][i].asDouble();
    SystemSize[1][i] = Root["SystemSize"][1][i].asDouble();
  }

  Volume = 1.0;
  HalfSystemSize = SystemSize[1] - SystemSize[0];
  for (uint32_t i = 0; i < D; i++) {
    Volume *= HalfSystemSize[i];
  }
  HalfSystemSize *= 0.5;

  Density = ParticlesNumber / Volume;

  for (uint32_t i = 0; i < ParticlesNumber; i++) {
    for (uint32_t j = 0; j < D; j++) {
      Particles[i].X[j] = Root["Particles"][starting_step][i][j].asDouble();
    }
  }

  if (StoredParticles) {
    for (uint32_t i = 0; i < SavedSteps; i++) { delete[] StoredParticles[i]; }
    delete[] StoredParticles;
  }

  StoredParticles = new Particle*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    StoredParticles[i] = new Particle[ParticlesNumber];
  }

  if (StoredStepSize) { delete[] StoredStepSize; }
  StoredStepSize = new double[SavedSteps];

  if (StoredDensity) { delete[] StoredDensity; }
  StoredDensity = new double[SavedSteps];

  if (StoredParticlesNumber) { delete[] StoredParticlesNumber; }
  StoredParticlesNumber = new double[SavedSteps];

  __PostLoadParticles(Root);
}

template <uint32_t D, class Particle> void MonteCarloSimulatorMuVT<D, Particle>::UpdateParticles()
{
  if (Particles == nullptr) return;
  double max_step_size = SystemSize[1][0] - SystemSize[0][0];
  for (uint32_t i = 1; i < D; i++) {
     max_step_size = SystemSize[1][i] - SystemSize[0][i] > max_step_size ? SystemSize[1][i] - SystemSize[0][i] : max_step_size;
  }
  StepSizeAdapter part_counter(StepSize, 1e-6 * ParticlesRadius, max_step_size * 0.5);

  Json::Value Progress;
  Json::FastWriter writer;
  __UpdateJsonOutput(Progress, 0, part_counter);

  uint32_t step = 0, print_steps = std::min(uint32_t(100),TotalSteps/100+1);
  for (uint32_t i = 0; i < TotalSteps; i++) {
    // Save spheres.
    if (SaveSystemInterval > 0 && i % SaveSystemInterval == 0) {
      __Measure();
      __SaveSystem(step);
      step++;
    }

    if (i % print_steps == 0) {
      __UpdateJsonOutput(Progress, i, part_counter);
      std::cout << writer.write(Progress);
    }

    // Try to move one particle.
    for (uint32_t j = 0; j < ParticleMoves; j++) {
      StepSize = part_counter.Update(__MoveParticle());
    }
  }

  if (SavedSteps > 0) {
    __SaveSystem(step);
    SavedSteps = step+1;
  }
  __UpdateJsonOutput(Progress, TotalSteps, part_counter);
  std::cout << writer.write(Progress);
}

template <uint32_t D, class Particle> void MonteCarloSimulatorMuVT<D, Particle>::SaveParticles(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = D;
  Root["ParticlesNumber"] = ParticlesNumber;
  Root["SavedSteps"] = SavedSteps;
  Root["ParticleMoves"] = ParticleMoves;
  Root["ParticlesRadius"] = ParticlesRadius;
  Root["Volume"] = Volume;
  Root["Density"] = Density;
  Root["Beta"] = Beta;

  Root["TotalSteps"] = TotalSteps;
  Root["SaveSystemInterval"] = SaveSystemInterval;

  for (uint32_t i = 0; i < D; i++) {
    Root["SystemSize"][0][i] = SystemSize[0][i];
    Root["SystemSize"][1][i] = SystemSize[1][i];
  }

  for (uint32_t i = 0; i < SavedSteps; i++) {
    Root["PartDelta"][i] = StoredStepSize[i];
  }

  for (uint32_t i = 0; i < SavedSteps; i++) {
    for (uint32_t j = 0; j < ParticlesNumber; j++) {
      for (uint32_t k = 0; k < D; k++) {
        Root["Particles"][i][j][k] = StoredParticles[i][j].X[k];
      }
    }
  }

  __PostSaveParticles(Root);

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
