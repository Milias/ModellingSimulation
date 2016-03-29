#pragma once
#include "mcmuvt.hpp"

double LJPotential(double r, double cut, double e_cut) {
  if (r > cut) {
    return 0.0;
  } else {
    double r6 = 1.0 / (r * r * r);
    return 4.0 * (r6 * (r6 - 1.0)) - e_cut;
  }
}

double LJVirial(double r, double cut) {
  if (r > cut) {
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

template <uint32_t D> class LennardJones : public MonteCarloSimulatorMuVT<D, LJParticle<D>>
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
  bool __AddParticle();
  bool __RemoveParticle();
  void __Measure();

public:
  LennardJones() {}
  ~LennardJones();
};

template <uint32_t D> LennardJones<D>::~LennardJones()
{
  if (StoredPressure) delete[] StoredPressure;
  if (StoredEnergy) delete[] StoredEnergy;
  if (StoredVirial) delete[] StoredVirial;
}

template <uint32_t D> void LennardJones<D>::__PostInitialize(Json::Value & root)
{
  RCut = root["RCut"].asDouble();

  if (StoredPressure) delete[] StoredPressure;
  if (StoredEnergy) delete[] StoredEnergy;
  if (StoredVirial) delete[] StoredVirial;

  StoredPressure = new double[this->SavedSteps];
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
    root["Energy"][i] = StoredEnergy[i];
    root["Virial"][i] = StoredVirial[i];
  }
}

template <uint32_t D> bool LennardJones<D>::__AddParticle()
{
  // Do not create new particles if the array is full.
  if (this->ParticlesNumber == this->MaxParticlesNumber) return false;
  LJParticle<D> test_part;

  // Creates a particle inside the box.
  this->Random.RandomVector(this->SystemSize[0], this->SystemSize[1], test_part.X);

  // Computes energy of the particle. TODO: check U(N+1)-U(N) = E+dE-E = test_part.Energy
  __ParticleEnergy(&test_part);

  // Random probability compared to acc(N -> N + 1).
  if (this->Random.RandomProb() < this->Volume / (this->ParticlesNumber + 1) * std::exp(this->Beta*(this->Mu - test_part.Energy))) {
    // Update system's energy and virial.
    Energy += test_part.Energy;
    Virial += test_part.Virial;

    // Put new particle in its rightful place.
    *this->ParticleToAdd = test_part;

    // Fill in the spot in the array.
    this->RemovedParticles[this->ParticleToAdd - this->Particles] = true;

    // If the array is being completely filled with this particle, just return true.
    if (this->ParticlesNumber + 1 == this->MaxParticlesNumber) {
      return true;
    }

    // Check if the next position of the array is free.
    if (!this->RemovedParticles[this->ParticlesNumber + 1]) {
      this->ParticleToAdd = this->Particles + this->ParticlesNumber + 1;
      return true;
    }

    // Otherwise check everything.
    for (uint32_t i = 0; i < this->MaxParticlesNumber; i++) {
      if (!this->RemovedParticles[i]) {
        this->ParticleToAdd = this->Particles + i;
        return true;
      }
    }
  }
  return false;
}

template <uint32_t D> bool LennardJones<D>::__RemoveParticle()
{
  // Do not remove any particle if the array is empty.
  if (this->ParticlesNumber == 0) return false;

  // Pick a particle.
  uint32_t index = this->Random.RandomInt();
  LJParticle<D> * test_part = nullptr;
  for (uint32_t i = 0, c = 0; i < this->MaxParticlesNumber; i++) {
    if (this->RemovedParticles[i]) {
      if (c == index) {
        test_part = this->Particles + i;
        break;
      }
      c++;
    }
    assert(c < this->ParticlesNumber);
  }

  // Computes energy of the particle.
  // TODO: check U(N-1)-U(N) = E-dE-E = -test_part.Energy
  __ParticleEnergy(test_part);

  // Random probability compared to acc(N -> N - 1).
  if (this->Random.RandomProb() < this->ParticlesNumber / this->Volume * std::exp(-this->Beta*(this->Mu - test_part->Energy))) {
    // Update system's energy and virial.
    Energy -= test_part->Energy;
    Virial -= test_part->Virial;

    // Change the place of the next created particle.
    this->ParticleToAdd = test_part;

    // Free the spot in the array.
    this->RemovedParticles[this->ParticleToAdd - this->Particles] = false;
    return true;
  }

  return false;
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
  Pressure = this->Density / this->Beta + Virial / 3.0 / this->Volume;
}
