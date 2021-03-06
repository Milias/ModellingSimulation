#pragma once
// NOT ENOUGH DATA TO GIVE A MEANINGFUL ANSWER.
#include "shared.h"
#include "mcnvt.hpp"

template <uint32_t D> class LJLastQuestion : public MonteCarloSimulatorNVT<D, LJParticle<D>>
{
protected:
  double
    RCut = 0.0,
    ECut = 0.0,
    Pressure = 0.0,
    Energy = 0.0,
    Virial = 0.0,
    * StoredPressure = nullptr,
    * StoredEnergy = nullptr,
    * StoredVirial = nullptr;

  void __PostInitialize(Json::Value & root);
  void __ParticleEnergy(LJParticle<D> * p);
  void __PostSaveSystem(uint32_t step);
  void __PostLoadParticles(Json::Value & root);
  void __PostSaveParticles(Json::Value & root);

  bool __MoveParticle();
  void __Measure();

public:
  LJLastQuestion() {}
  ~LJLastQuestion();
};

template <uint32_t D> LJLastQuestion<D>::~LJLastQuestion()
{
  if (StoredPressure) delete[] StoredPressure;
  if (StoredEnergy) delete[] StoredEnergy;
  if (StoredVirial) delete[] StoredVirial;
}

template <uint32_t D> void LJLastQuestion<D>::__PostInitialize(Json::Value & root)
{
  RCut = root["RCut"].asDouble();
  ECut = 4.0 * ( std::pow(1.0 / RCut, 12) - std::pow(1.0 / RCut, 6) );

  if (StoredPressure) delete[] StoredPressure;
  if (StoredEnergy) delete[] StoredEnergy;
  if (StoredVirial) delete[] StoredVirial;

  StoredPressure = new double[this->SavedSteps];
  StoredEnergy = new double[this->SavedSteps];
  StoredVirial = new double[this->SavedSteps];
}

template <uint32_t D> void LJLastQuestion<D>::__ParticleEnergy(LJParticle<D> * p)
{
  uint32_t this_part_index = p - this->Particles;
  double dist;
  p->Energy = 0.0;
  p->Virial = 0.0;
  for (uint32_t i = 0; i < this->ParticlesNumber; i++) {
    if (this_part_index == i) continue;
    dist = this->__ComputeDistance2(p->X, this->Particles[i].X);
    p->Energy += LJPotential(dist, RCut, ECut);
    p->Virial += LJVirial(dist, RCut);
  }
}

template <uint32_t D> void LJLastQuestion<D>::__PostSaveSystem(uint32_t step)
{
  StoredPressure[step] = Pressure;
  StoredEnergy[step] = Energy;
  StoredVirial[step] = Virial;
}

template <uint32_t D> void LJLastQuestion<D>::__PostLoadParticles(Json::Value & root)
{
  Energy = 0.0;
  Virial = 0.0;
  for (uint32_t i = 0; i < this->ParticlesNumber; i++) {
    __ParticleEnergy(this->Particles + i);
    Energy += this->Particles[i].Energy;
    Virial += this->Particles[i].Virial;
  }

  // Because of overcounting. TODO: more efficiently ?
  Energy *= 0.5;
  Virial *= 0.5;
}

template <uint32_t D> void LJLastQuestion<D>::__PostSaveParticles(Json::Value & root)
{
  for (uint32_t i = 0; i < this->SavedSteps; i++) {
    root["Pressure"][i] = StoredPressure[i];
    root["Energy"][i] = StoredEnergy[i];
    root["Virial"][i] = StoredVirial[i];
  }
}

template <uint32_t D> bool LJLastQuestion<D>::__MoveParticle()
{
  LJParticle<D> * picked_part = this->Particles + this->Random.RandomInt();

  __ParticleEnergy(picked_part);
  LJParticle<D> old_part(picked_part);

  for (uint32_t i = 0; i < D; i++) {
    picked_part->X[i] += this->StepSize * this->Random.RandomDouble();
  }
  picked_part->X.Wrap(this->SystemSize[0], this->SystemSize[1]);

  __ParticleEnergy(picked_part);

  double dE = picked_part->Energy - old_part.Energy;
  if (dE < 0.0 || this->Random.RandomProb() < std::exp(-this->Beta*dE)) {
    Energy += dE;
    Virial += picked_part->Virial - old_part.Virial;
    return true;
  }

  *picked_part = old_part;
  return false;
}

template <uint32_t D> void LJLastQuestion<D>::__Measure()
{
  Pressure = this->Density / this->Beta + Virial / 3.0 / this->Volume;
}
