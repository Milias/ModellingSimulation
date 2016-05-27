#pragma once
#include "shared.h"
#include <boost/numeric/odeint.hpp>

using namespace boost::numeric::odeint;
typedef std::vector<double> state_type;
typedef runge_kutta_cash_karp54< state_type > error_stepper_type;

class Elevator
{
private:
  bool Initialize = false;
  bool FixedEarthBoundary = false;

  const uint32_t
    Dimensions = 3;

  uint32_t
    ChainSize = 0,
    TotalSteps = 0,
    SavedSteps = 0;

  double
    L0 = 0.0,
    GM = 0.0,
    T0 = 0.0,
    Tf = 0.0,
    Dt = 0.0,
    WPlanet = 0.0,
    RSurface = 0.0,
    FrictA = 0.0,
    FrictB = 0.0,
    FrictC = 0.0,
    * AnchorAngles = nullptr,
    * SprK = nullptr,
    * RotK = nullptr,
    * Inertia = nullptr,
    * Mass = nullptr;

  state_type * State = nullptr;

  std::vector<double*> StoredStates;

  void __RHS(state_type &y, state_type &dy, double t);
  double __Distance(const state_type &y, uint32_t i, uint32_t j);
  double __Modulus2(const state_type &y, uint32_t i);
  double __Alpha(const state_type &y, uint32_t i);

  void __EarthFixedBoundary(state_type &y, state_type &dy, const double t);
  void __EarthFreeBoundary(state_type &y, state_type &dy, const double t);
  void __SpaceFreeBoundary(state_type &y, state_type &dy, const double t);

  void __StoreState(const state_type &y, const double t);

public:
  Elevator() {}
  ~Elevator() {
    delete[] SprK;
    delete[] RotK;
    delete[] Inertia;
    delete[] Mass;
    delete[] AnchorAngles;
    delete State;
  }

  virtual void InitializeFromFile(char const * filename);
  void LoadSystem(char const * filename);
  void SaveSystem(char const * filename);

  void Integrate();
};
