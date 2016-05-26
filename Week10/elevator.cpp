#include "elevator.hpp"

void Elevator::__RHS(state_type &y, state_type &dy, const double t)
{
  /*
    y[8*i] = theta_i
    y[8*i+4] = theta_i'
    y[8*i+1+j] = r_ij
    y[8*i+5+j] = r_ij'

    dy[8*i] = theta_i'
    dy[8*i+4] = theta_i''
    dy[8*i+1+j] = r_ij'
    dy[8*i+5+j] = r_ij''
  */

  // Boundaries
  if (FixedEarthBoundary) { __EarthFixedBoundary(y, dy, t); }
  else { __EarthFreeBoundary(y, dy, t); }
  __SpaceFreeBoundary(y, dy, t);

  // Bulk
  for (uint32_t i = 1; i < ChainSize-1; i++) {
    dy[8*i] = y[8*i+4];

    dy[8*i+4] = (RotK[i-1]*y[8*(i-1)]-(RotK[i]+RotK[i-1])*y[8*i]-RotK[i+1]*y[8*(i+1)])/Inertia[i];

    for (uint32_t j = 0; j < Dimensions; j++) {
      dy[8*i+1+j] = y[8*i+5+j];

      dy[8*i+5+j] =
      -__Alpha(y,i)/Mass[i]*y[8*i+5+j]
      +SprK[i]/Mass[i]*(__Distance(y,i+1,i)-L0)*(y[8*(i+1)+1+j]-y[8*i+1+j])/__Distance(y,i+1,i)
      -SprK[i-1]/Mass[i]*(__Distance(y,i,i-1)-L0)*(y[8*i+1+j]-y[8*(i-1)+1+j])/__Distance(y,i,i-1)
      -GM/std::pow(__Modulus2(y, i),1.5)*y[8*i+1+j];
    }
  }
}

double Elevator::__Distance(const state_type &y, uint32_t i, uint32_t j)
{
  double d = 0.0;
  for (uint32_t k = 0; k < Dimensions; k++) {
    d += std::pow(y[8*i+1+k]-y[8*j+1+k],2);
  }
  return std::sqrt(d);
}

double Elevator::__Modulus2(const state_type &y, uint32_t i)
{
  double d = 0.0;
  for (uint32_t k = 0; k < Dimensions; k++) {
    d += y[8*i+1+k]*y[8*i+1+k];
  }
  return d;
}

double Elevator::__Alpha(const state_type &y, uint32_t i)
{
  double d = FrictB*(RSurface-std::sqrt(__Modulus2(y, i)));
  //printf("fb: %f, rs: %f, dist: %f, d: %f\n", FrictB, RSurface, std::sqrt(__Modulus2(y, i)), d);
  if (abs(d) < 40.0) {
    return FrictA*std::exp(d) + FrictC;
  }
  return FrictC;
}

void Elevator::__EarthFixedBoundary(state_type &y, state_type &dy, const double t)
{
  /*
    y[0] = theta_i
    y[4] = theta_i'
    y[1+j] = r_ij
    y[5+j] = r_ij'

    dy[0] = theta_i'
    dy[4] = theta_i''
    dy[1+j] = r_ij'
    dy[5+j] = r_ij''
  */

  dy[0] = 0.0;
  dy[4] = 0.0;

  y[1] = -RSurface*std::sin(WPlanet*t);
  y[2] = 0.0;
  y[3] = RSurface*std::cos(WPlanet*t);

  for (uint32_t j = 0; j < Dimensions; j++) {
    dy[1+j] = 0.0;
    dy[5+j] = 0.0;
  }
}

void Elevator::__EarthFreeBoundary(state_type &y, state_type &dy, const double t)
{
  /*
    y[0] = theta_i
    y[4] = theta_i'
    y[1+j] = r_ij
    y[5+j] = r_ij'

    dy[0] = theta_i'
    dy[4] = theta_i''
    dy[1+j] = r_ij'
    dy[5+j] = r_ij''
  */

  dy[0] = y[4];
  dy[4] = RotK[0]/Inertia[0]*(y[1]-y[0]);

  for (uint32_t j = 0; j < Dimensions; j++) {
    dy[1+j] = y[5+j];
    dy[5+j] =
    -__Alpha(y, 0)/Mass[0]*y[5+j]
    +SprK[0]/Mass[0]*(__Distance(y,1,0)-L0)*(y[8+1+j]-y[1+j])/__Distance(y,1,0)
    -GM/std::pow(__Modulus2(y, 0),1.5)*y[1+j];
  }
}

void Elevator::__SpaceFreeBoundary(state_type &y, state_type &dy, const double t)
{
  /*
    y[8*(ChainSize-1)] = theta_i
    y[8*(ChainSize-1)+4] = theta_i'
    y[8*(ChainSize-1)+1+j] = r_ij
    y[8*(ChainSize-1)+5+j] = r_ij'

    dy[8*(ChainSize-1)] = theta_i'
    dy[8*(ChainSize-1)+4] = theta_i''
    dy[8*(ChainSize-1)+1+j] = r_ij'
    dy[8*(ChainSize-1)+5+j] = r_ij''
  */

  dy[8*(ChainSize-1)] = y[8*(ChainSize-1)+4];
  dy[8*(ChainSize-1)+4] = RotK[(ChainSize-1)]/Inertia[(ChainSize-1)]*(y[(ChainSize-2)]-y[(ChainSize-1)]);

  for (uint32_t j = 0; j < Dimensions; j++) {
    dy[8*(ChainSize-1)+1+j] = y[8*(ChainSize-1)+5+j];
    dy[8*(ChainSize-1)+5+j] =
      //-__Alpha(y,ChainSize-1)/Mass[8*(ChainSize-1)]*y[8*(ChainSize-1)+5+j]
      SprK[ChainSize-1]/Mass[ChainSize-1]*(__Distance(y, ChainSize-1, ChainSize-2)-L0)*(y[8*(ChainSize-2)+1+j]-y[8*(ChainSize-1)+1+j])/__Distance(y, ChainSize-1, ChainSize-2)
      -GM/std::pow(__Modulus2(y, ChainSize-1),1.5)*y[8*(ChainSize-1)+1+j]
    ;
  }
}

void Elevator::__StoreState(const state_type &y, const double t)
{
  double* state = new double[8*ChainSize+1];
  state[0] = t;
  for (uint32_t i = 1; i <= 8*ChainSize; i++) {
    state[i] = y[i-1];
  }
  StoredStates.push_back(state);
  TotalSteps++;
  SavedSteps++;
}

void Elevator::InitializeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  GM = Root["GM"].asDouble();
  T0 = Root["T0"].asDouble();
  Tf = Root["Tf"].asDouble();
  Dt = Root["Dt"].asDouble();

  WPlanet = Root["WPlanet"].asDouble();
  RSurface = Root["RSurface"].asDouble();
  FixedEarthBoundary = Root["Fixed"].asBool();

  FrictA = Root["FrictA"].asDouble();
  FrictB = Root["FrictB"].asDouble();
  FrictC = Root["FrictC"].asDouble();

  Initialize = true;
}

void Elevator::LoadSystem(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  // Load basic quantities.
  L0 = Root["L0"].asDouble();
  ChainSize = Root["ChainSize"].asUInt();

  // Allocate position-dependent variables.
  SprK = new double[ChainSize];
  RotK = new double[ChainSize];
  Inertia = new double[ChainSize];
  Mass = new double[ChainSize];

  State = new state_type(8*ChainSize);

  // Load 'em.
  for (uint32_t i = 0; i < ChainSize; i++) {
    // Material constants.
    SprK[i] = Root["SprK"][i].asDouble();
    RotK[i] = Root["RotK"][i].asDouble();
    Inertia[i] = Root["Inertia"][i].asDouble();
    Mass[i] = Root["Mass"][i].asDouble();

    // Dynamical initial conditions.
    /*
      State[8*i] = theta_i
      State[8*i+4] = theta_i'
      State[8*i+1+j] = r_ij
      State[8*i+5+j] = r_ij'
    */

    (*State)[8*i] = Root["Angle"][i].asDouble();
    (*State)[8*i+4] = Root["AngularVelocity"][i].asDouble();
    for (uint32_t j = 0; j < Dimensions; j++) {
      (*State)[8*i+1+j] = Root["Position"][i][j].asDouble();
      (*State)[8*i+5+j] = Root["Velocity"][i][j].asDouble();
    }
  }
}

void Elevator::SaveSystem(char const * filename)
{
  Json::Value Root;

  Root["ChainSize"] = ChainSize;
  Root["TotalSteps"] = TotalSteps;
  Root["SavedSteps"] = SavedSteps;

  Root["L0"] = L0;
  Root["GM"] = GM;
  Root["T0"] = T0;
  Root["Tf"] = Tf;
  Root["Dt"] = Dt;

  for (uint32_t i = 0; i < ChainSize; i++) {
    Root["SprK"][i] = SprK[i];
    Root["RotK"][i] = RotK[i];
    Root["Inertia"][i] = Inertia[i];
    Root["Mass"][i] = Mass[i];
  }

  // [Step][ChainNode][Dimension]
  for (uint32_t i = 0; i < SavedSteps; i++) {
    Root["Time"][i] = StoredStates[i][0];
    for (uint32_t j = 0; j < ChainSize; j++) {
      Root["Angle"][i][j] = StoredStates[i][8*j+1];
      Root["AngularVelocity"][i][j] = StoredStates[i][8*j+5];
      for (uint32_t k = 0; k < Dimensions; k++) {
        Root["Position"][i][j][k] = StoredStates[i][8*j+2+k];
        Root["Velocity"][i][j][k] = StoredStates[i][8*j+6+k];
      }
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}

void Elevator::Integrate()
{
  runge_kutta4< state_type > stepper;
  integrate_const(
    stepper,
    [this](state_type &y, state_type &dy, double t) { this->__RHS(y, dy, t); },
    *State, T0, Tf, Dt,
    [this](const state_type &y, const double t) { this->__StoreState(y, t); }
  );
}
