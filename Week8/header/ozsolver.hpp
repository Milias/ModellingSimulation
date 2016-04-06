#pragma once
#include "shared.h"
#include "mcnvt.hpp"
#include "part.hpp"

extern "C" {
  #include "../sinfft.c"
}

template <uint32_t D> class OZSolverHardSpheres : public MonteCarloSimulatorNVT<D, GenericParticle<D>>
{
private:
  double
    * Gamma_r = nullptr,
    * C_r = nullptr,
    * Gamma_k = nullptr,
    * C_k = nullptr,
    ** StoredGamma = nullptr,
    DeltaR = 0.0,
    DeltaQ = 0.0,
    Tolerance = 0.0;

  uint32_t GridPoints = 0, MaxIterations = 1000;

  void __ComputeNewGamma();
  void __ComputeClosure();

public:
  OZSolver() {}
  ~OZSolver();
};

template <uint32_t D> void OZSolverHardSpheres<D>::__ComputeNewGamma()
{
  for (uint32_t i = 0; i < MaxIterations; i++) {
    __ComputeClose();
    for (uint32_t j = 0; j < GridPoints; j++) { C_k[j] = C_r[j]; }
    sinft(C_k, GridPoints);
    for (uint32_t j = 0; j < GridPoints; j++) { C_k[j] = C_r[j]; }
  }
}
