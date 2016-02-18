#include "randomwalk.h"

RandomWalk::RandomWalk(uint32_t dim) : Dimensions(dim)
{
  Generator.seed(time(NULL));
  RandomRealdistribution = std::uniform_real_distribution<double>(0.0,1.0);

  RandomDouble = std::bind(RandomRealdistribution, Generator);
}

RandomWalk::~RandomWalk()
{
  if (StepLocations) delete[] StepLocations;
}

Point RandomWalk::RandomUnitaryVector()
{
  Point r(Dimensions);
  double s = 10;
  for(; s > 1;) {
    for (uint32_t i = 0; i < Dimensions; i++) {
      r[i] = 1.0 - 2.0 * RandomDouble();
    }
    s = r*r;
  }
  r /= s;
  return r;
}

Point * RandomWalk::SimpleRandomWalk(double delta, uint32_t steps)
{
  if (StepLocations) delete[] StepLocations;
  StepLocations = new Point[steps];
  StepLength = delta;

  StepLocations[0] = RandomUnitaryVector() * StepLength;
  for (uint32_t i = 1; i < steps; i++) {
    StepLocations[i] = RandomUnitaryVector() * StepLength + StepLocations[i - 1];
  }

  StepNumber = steps;
  return StepLocations;
}

double * RandomWalk::SimpleComputeRSquared(double delta, uint32_t steps, uint32_t nwalks)
{
  if (RSquared) delete[] RSquared;
  RSquared = new double[steps];

  WalksNumber = nwalks;
  StepLength = delta;
  StepNumber = steps;
  BoxSize = 0.0;

  Point * walk;
  for (uint32_t i = 0; i < steps; i++) RSquared[i] = 0.0;

  for (uint32_t i = 0; i < nwalks; i++) {
    walk = SimpleRandomWalk(delta, steps);
    for (uint32_t j = 0; j < steps; j++) {
      RSquared[j] += sqrt(walk[j]*walk[j]);
    }
  }

  for (uint32_t i = 0; i < steps; i++) RSquared[i] /= nwalks;

  return RSquared;
}

Point * RandomWalk::PeriodicRandomWalk(double delta, uint32_t steps, double size)
{
  if (StepLocations) delete[] StepLocations;
  StepLocations = new Point[steps];

  StepLength = delta;
  BoxSize = size;
  StepNumber = steps;

  StepLocations[0] = RandomUnitaryVector() * StepLength;
  for (uint32_t i = 1; i < steps; i++) {
    StepLocations[i] = (RandomUnitaryVector() * StepLength + StepLocations[i - 1]) % (BoxSize / 2.0);
  }

  return StepLocations;
}

double * RandomWalk::PeriodicComputeRSquared(double delta, uint32_t steps, uint32_t nwalks, double size)
{
  if (RSquared) delete[] RSquared;
  RSquared = new double[steps];

  WalksNumber = nwalks;
  StepLength = delta;
  StepNumber = steps;
  BoxSize = size;

  for (uint32_t i = 0; i < steps; i++) RSquared[i] = 0.0;

  Point * walk;
  for (uint32_t i = 0; i < nwalks; i++) {
    walk = PeriodicRandomWalk(delta, steps, size);
    for (uint32_t j = 0; j < steps; j++) {
      RSquared[j] += sqrt(walk[j]*walk[j]);
    }
  }

  for (uint32_t i = 0; i < steps; i++) RSquared[i] /= nwalks;

  return RSquared;
}

void RandomWalk::SaveWalkData(char const * filename)
{
  Json::Value Root;
  Root["Type"] = "Walk Data";
  Root["Dimensions"] = Dimensions;
  Root["StepLength"] = StepLength;
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
  Root["Dimensions"] = Dimensions;
  Root["StepLength"] = StepLength;
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
