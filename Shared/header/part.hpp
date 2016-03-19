#pragma once
#include "vector.hpp"

template <uint32_t D> struct GenericParticle
{
  Vector<D> X;

  GenericParticle() {}
  GenericParticle(const GenericParticle & p) : X(p.X) {}
  GenericParticle(const GenericParticle * p) : X(p->X) {}

  virtual GenericParticle & operator=(const GenericParticle & p) {
    X = p.X;
    return *this;
  }

  virtual ~GenericParticle() {}
};
