#pragma once
#include "mcnvt.hpp"
#include "part.hpp"

double LJPotential(double r, double cut, double e_cut) {
  if (r > cut*cut) {
    return 0.0;
  } else {
    double r6 = 1.0 / (r * r * r);
    return 4.0 * (r6 * (r6 - 1.0) - e_cut);
  }
}

double LJVirial(double r, double cut) {
  if (r > cut*cut) {
    return 0.0;
  } else {
    double r6 = 1.0 / (r * r * r);
    return 24.0 * r6 * (2.0 * r6 - 1.0);
  }
}

template <uint32_t D> struct LJParticle : GenericParticle<D>
{
  double Energy = 0.0, Virial = 0.0;

  LJParticle() {}
  LJParticle(const LJParticle & p) : Energy(p.Energy), Virial(p.Virial) {
    this->X = p.X;
  }
  LJParticle(const LJParticle * p) : Energy(p->Energy), Virial(p->Virial) {
    this->X = p->X;
  }

  LJParticle & operator=(const LJParticle & p) {
    this->X = p.X;
    Energy = p.Energy;
    Virial = p.Virial;
    return *this;
  }
};

template <uint32_t D> class LennardJones : public MonteCarloSimulatorNVT<D, LJParticle<D>>
{
protected:
  uint32_t
    MuExcessTests = 0;

  double
    RCut = 0.0,
    ECut = 0.0,
    Pressure = 0.0,
    MuExcess = 0.0,
    Energy = 0.0,
    Virial = 0.0,
    * StoredPressure = nullptr,
    * StoredMuExcess = nullptr,
    * StoredEnergy = nullptr,
    * StoredVirial = nullptr;

  void __PostInitialize(Json::Value & root);
  void __ParticleEnergy(LJParticle<D> * p);
  void __PostSaveSystem(uint32_t step);
  void __PostLoadParticles(Json::Value & root);
  void __PostSaveParticles(Json::Value & root);

  double __TestAddParticle();
  double __ComputeMuExcess();
  bool __MoveParticle();
  void __Measure();

public:
  LennardJones() {}
  ~LennardJones();
};

template <uint32_t D> LennardJones<D>::~LennardJones()
{
  if (StoredPressure) delete[] StoredPressure;
  if (StoredMuExcess) delete[] StoredMuExcess;
  if (StoredEnergy) delete[] StoredEnergy;
  if (StoredVirial) delete[] StoredVirial;
}

template <uint32_t D> void LennardJones<D>::__PostInitialize(Json::Value & root)
{
  MuExcessTests = root["MuExcessTests"].asUInt();
  RCut = root["RCut"].asDouble();

  if (StoredPressure) delete[] StoredPressure;
  if (StoredMuExcess) delete[] StoredMuExcess;
  if (StoredEnergy) delete[] StoredEnergy;
  if (StoredVirial) delete[] StoredVirial;

  StoredPressure = new double[this->SavedSteps];
  StoredMuExcess = new double[this->SavedSteps];
  StoredEnergy = new double[this->SavedSteps];
  StoredVirial = new double[this->SavedSteps];
}

template <uint32_t D> void LennardJones<D>::__ParticleEnergy(LJParticle<D> * p)
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

template <uint32_t D> void LennardJones<D>::__PostSaveSystem(uint32_t step)
{
  StoredPressure[step] = Pressure;
  StoredMuExcess[step] = MuExcess;
  StoredEnergy[step] = Energy;
  StoredVirial[step] = Virial;
}

template <uint32_t D> void LennardJones<D>::__PostLoadParticles(Json::Value & root)
{
  ECut = 4.0 * ( std::pow(1.0 / RCut, 12) - std::pow(1.0 / RCut, 6) );

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

template <uint32_t D> void LennardJones<D>::__PostSaveParticles(Json::Value & root)
{
  for (uint32_t i = 0; i < this->SavedSteps; i++) {
    root["Pressure"][i] = StoredPressure[i];
    root["MuExcess"][i] = StoredMuExcess[i];
    root["Energy"][i] = StoredEnergy[i];
    root["Virial"][i] = StoredVirial[i];
  }
}

template <uint32_t D> double LennardJones<D>::__TestAddParticle()
{
  LJParticle<D> test_part;

  // Creates a particle inside the box.
  this->Random.RandomVector(this->SystemSize[0], this->SystemSize[1], test_part.X);

  // Computes energy of the particle.
  __ParticleEnergy(&test_part);

  return test_part.Energy;
}

template <uint32_t D> double LennardJones<D>::__ComputeMuExcess()
{
  double average = 0.0;
  for (uint32_t i = 0; i < MuExcessTests; i++) {
    average += std::exp(-this->Beta*__TestAddParticle());
  }
  return average > 0 ? -std::log(average/MuExcessTests)/this->Beta : MuExcess;
}

template <uint32_t D> bool LennardJones<D>::__MoveParticle()
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

template <uint32_t D> void LennardJones<D>::__Measure()
{
  Pressure = this->Density  / this->Beta + Virial / 3.0 / this->Volume;
  MuExcess = __ComputeMuExcess();
}
