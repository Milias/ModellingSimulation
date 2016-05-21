#pragma once
#include "elevator.hpp"

void Elevator::__RHS(const double* &y, double* &dy, const double t)
{
  /*
    y[8*i] = theta_i
    y[8*i+1+j] = r_ij
    y[8*i+4] = theta_i'
    y[8*i+4+j] = r_ij'

    dy[8*i] = theta_i'
    dy[8*i+1+j] = r_ij'
    dy[8*i+4] = theta_i''
    dy[8*i+4+j] = r_ij''
  */

  __EarthBoundary(y, dy, t);
  __SpaceBoundary(y, dy, t);

  // Bulk
  for (uint32_t i = 1; i < ChainSize-1; i++) {
    dy[8*i] = y[8*i+4];

    dy[8*i+4] = (RotK[i-1]*y[8*(i-1)]-(RotK[i]+RotK[i-1])*y[8*i]-RotK[i+1]*y[8*(i+1)])/Inertia[i];

    for (uint32_t j = 0; j < Dimensions; j++) {
      dy[8*i+1+j] = y[8*i+4+j];

      dy[8*i+4+j] = SprK[i]/Mass[i]*(1-L_0/__Distance(y,i+1,i))*(y[8*(i+1)+1+j]-y[8*i+1+j])
      -SprK[i-1]/Mass[i]*(1-L_0/__Distance(y,i,i-1))*(y[8*i+1+j]-y[8*(i-1)+1+j])
      +GM/std::pow(__Modulus2(y, i),1.5)*y[8*i+1+j];
    }
  }
}

void Elevator::__Distance(double * y, uint32_t i, uint32_t j)
{
  double d = 0.0;
  for (uint32_t k = 0; k < Dimensions; k++) {
    d += std::pow(y[8*i+1+k]-y[8*j+1+k],2)
  }
  return std::sqrt(d);
}

void Elevator::__Modulus2(double * y, uint32_t i)
{
  double d = 0.0;
  for (uint32_t k = 0; k < Dimensions; k++) {
    d += std::pow(y[8*i+1+k]-y[8*j+1+k],2)
  }
  return d;
}

void Elevator::__EarthBoundary(const double * y, double * dy, const double t)
{

}

void Elevator::__SpaceBoundary(const double * y, double * dy, const double t)
{

}


void Elevator::InitializeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  Initialize = true;
}

void Elevator::LoadSystem(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
