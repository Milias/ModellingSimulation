#pragma once
#include "part.hpp"

double LJPotential(double r, double cut, double e_cut) {
  if (r > cut*cut) {
    return 0.0;
  } else {
    double r6 = 1.0 / (r * r * r);
    return 4.0 * (r6 * (r6 - 1.0)) - e_cut;
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
