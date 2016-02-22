#include "randomwalk.h"

RandomWalk::RandomWalk(uint32_t dim) : Dimensions(dim), StepLocations(NULL), RSquared(NULL)
{
  Generator.seed(DefaultClock::now().time_since_epoch().count());
  RandomRealDistribution = std::uniform_real_distribution<double>(-1.0,1.0);

  RandomDouble = std::bind(RandomRealDistribution, Generator);
}

RandomWalk::~RandomWalk()
{
  if (StepLocations) delete[] StepLocations;
  if (RSquared) delete[] RSquared;
}

Point RandomWalk::RandomUnitaryVector()
{
  Point r(Dimensions);
  return RandomUnitaryVector(r);
}

Point & RandomWalk::RandomUnitaryVector(Point & p)
{
  double s = 10;
  for(; s > 1;) {
    // Generates random vectors in [0, 1]^N until
    // one lies inside a sphere of radius one.
    for (uint32_t i = 0; i < Dimensions; i++) {
      p[i] = RandomDouble();
    }
    s = p * p;
  }
  p /= sqrt(s);
  return p;
}

Point * RandomWalk::ComputeRandomWalk(uint32_t steps, double size)
{
  if (StepLocations == NULL || StepNumber != steps) {
    delete[] StepLocations;
    StepLocations = new Point[steps];
  }

  StepNumber = steps;
  BoxSize = size;

  for (uint32_t i = 0; i < StepNumber; i++) { StepLocations[i].Init(Dimensions); }

  RandomUnitaryVector(StepLocations[0]);
  if (BoxSize > 0) {
    for (uint32_t i = 1; i < StepNumber; i++) {
      RandomUnitaryVector(StepLocations[i]);
      StepLocations[i] += StepLocations[i - 1];
      StepLocations[i] %= BoxSize / 2.0;
    }
  } else {
    for (uint32_t i = 1; i < StepNumber; i++) {
      RandomUnitaryVector(StepLocations[i]);
      StepLocations[i] += StepLocations[i - 1];
    }
  }

  return StepLocations;
}

double * RandomWalk::ComputeRSquared(uint32_t steps, uint32_t nwalks, double size)
{
  if (RSquared == NULL || StepNumber != steps) {
    delete[] RSquared;
    RSquared = new double[steps];
  }

  WalksNumber = nwalks;
  StepNumber = steps;
  BoxSize = size;

  for (uint32_t i = 0; i < StepNumber; i++) RSquared[i] = 0.0;

  Point * walk;
  for (uint32_t i = 0; i < WalksNumber; i++) {
    walk = ComputeRandomWalk(StepNumber, BoxSize);
    for (uint32_t j = 0; j < StepNumber; j++) {
      RSquared[j] += walk[j]*walk[j];
    }
  }

  for (uint32_t i = 0; i < steps; i++) RSquared[i] /= WalksNumber;

  return RSquared;
}

void RandomWalk::SaveWalkData(char const * filename)
{
  Json::Value Root;
  Root["Type"] = "Walk Data";
  Root["Legend"] = BoxSize > 0.0 ? "With periodic boundary conditions" : "Without periodic boundary conditions";
  Root["Dimensions"] = Dimensions;
  Root["StepNumber"] = StepNumber;
  Root["BoxSize"] = BoxSize;

  for (uint32_t i = 0; i < Dimensions; i++) {
    Root["Data"][i] = Json::Value(Json::arrayValue);
  }

  for (uint32_t i = 0; i < StepNumber; i++) {
    for (uint32_t j = 0; j < Dimensions; j++) {
      Root["Data"][j].append(StepLocations[i][j]);
    }
  }

  Json::StyledStreamWriter writer;
  std::ofstream savefile(filename);
  writer.write(savefile, Root);
  savefile.close();
}

void RandomWalk::SaveRData(char const * filename)
{
  Json::Value Root;
  Root["Type"] = "R Squared Data";
  Root["Legend"] = BoxSize > 0.0 ? "With periodic boundary conditions" : "Without periodic boundary conditions";
  Root["Dimensions"] = Dimensions;
  Root["WalksNumber"] = WalksNumber;
  Root["StepNumber"] = StepNumber;
  Root["BoxSize"] = BoxSize;

  Root["Data"] = Json::Value(Json::arrayValue);

  for (uint32_t i = 0; i < StepNumber; i++) {
    Root["Data"][i].append(RSquared[i]);
  }

  Json::StyledStreamWriter writer;
  std::ofstream savefile(filename);
  writer.write(savefile, Root);
  savefile.close();
}
