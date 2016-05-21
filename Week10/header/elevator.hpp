#pragma once
#include "shared.h"
#include <boost/numeric/odeint.hpp>

using namespace boost::numeric::odeint;
typedef runge_kutta_dopri5<double> stepper_type;

class Elevator
{
private:
  bool Initialize = false;

  const uint32_t
    Dimensions = 3;

  uint32_t
    ChainSize = 0;

  double
    L_0 = 0.0,
    GM = 0.0,
    * SprK = nullptr,
    * RotK = nullptr,
    * R = nullptr,
    * Rho = nullptr,
    * Inertia = nullptr,
    * Mass = nullptr;

  void __RHS(const state_type &y, state_type &dy, const double t);
  double __Distance(double * y, uint32_t i, uint32_t j);
  double __Modulus2(double * y, uint32_t i);

  void __EarthBoundary(const double* &y, double * dy, const double t);
  void __SpaceBoundary(const double* &y, double * dy, const double t);

public:
  Elevator() {}
  ~Elevator() {}

  virtual void InitializeFromFile(char const * filename);
  void LoadSystem(char const * filename);
  void SaveSystem(char const * filename);
};
