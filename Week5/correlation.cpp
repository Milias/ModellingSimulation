#include "correlation.h"

Correlation::Correlation() {}

Correlation::~Correlation()
{
  if (Spheres) delete[] Spheres;
  if (SystemSize) delete[] SystemSize;
  if (SystemSizeHalf) delete[] SystemSizeHalf;
  if (Distances) delete[] Distances;
  if (BinDistances) delete[] BinDistances;
  if (BoxParticleDensity) delete[] BoxParticleDensity;
  if (NormalizedDensity) delete[] NormalizedDensity;
  if (DistancesCount) delete[] DistancesCount;
}

double Correlation::__Nideal(uint32_t step, uint32_t bin)
{
  double V = Dimensions % 2 == 0 ?
    std::pow(pi, Dimensions) / factorial(Dimensions) :
    2 * factorial(Dimensions) * std::pow(4 * pi, Dimensions) / factorial(2 * Dimensions + 1);
  return BoxParticleDensity[step]*V*(std::pow(BinDistances[bin] + BinWidth, Dimensions)-std::pow(BinDistances[bin],Dimensions));
}

void Correlation::__InitializeBins()
{
  if (BinDistances) delete[] BinDistances;
  if (DistancesCount) delete[] DistancesCount;
  if (NormalizedDensity) delete[] NormalizedDensity;
  BinDistances = new double[SavedSteps * BinsNumber];
  DistancesCount = new uint32_t[SavedSteps * BinsNumber];
  NormalizedDensity = new double[SavedSteps * BinsNumber];

  for (uint32_t j = 0; j < BinsNumber; j++) {
    BinDistances[j] = IDistance + j * BinWidth;
    for (uint32_t i = 0; i < SavedSteps; i++) {
      DistancesCount[i * BinsNumber + j] = 0;
    }
  }
}

void Correlation::__ComputeDistances(uint32_t step)
{
  Point p1(Dimensions);
  for (uint32_t i = 0, k = 0; i < SpheresNumber; i++) {
    for (uint32_t j = i + 1; j < SpheresNumber; j++, k++) {
      p1 = Spheres[step * SpheresNumber + i];
      p1 -= Spheres[step * SpheresNumber + j];
      p1 %= SystemSizeHalf[step];
      Distances[step * DistancesNumber + k] = std::sqrt(p1 * p1);
    }
  }
}

void Correlation::__CountDistances(uint32_t step)
{
  for (uint32_t i = 0; i < DistancesNumber; i++) {
    for (uint32_t j = 0; j < BinsNumber; j++) {
<<<<<<< HEAD
      if (BinDistances[j] >= Distances[step * DistancesNumber + i]) {
        DistancesCount[step * BinsNumber + j] += 2;
=======
      if (BinDistances[j] > Distances[step * DistancesNumber + i]) {
        DistancesCount[step * BinsNumber + j]++;
>>>>>>> 8ebc1ce7c21bb664fa948fa5f207337baaed4f64
        break;
      }
    }
  }
}

void Correlation::__NormalizeCounts(uint32_t step)
{
  for (uint32_t i = 0; i < BinsNumber; i++) {
    NormalizedDensity[step * BinsNumber + i] =
<<<<<<< HEAD
       DistancesCount[step * BinsNumber + i] / __Nideal(step, i) / SpheresNumber;
=======
       DistancesCount[step * BinsNumber + i] / SpheresNumber / __Nideal(step, i);
>>>>>>> 8ebc1ce7c21bb664fa948fa5f207337baaed4f64
  }
}

void Correlation::ComputeHistogram(double r0, double rf, uint32_t bins)
{
  IDistance = r0;
  FDistance = rf;

  if (bins > 0) {
    BinsNumber = bins;
    BinWidth = (FDistance - IDistance) / BinsNumber;
  }

  printf("r0: %f, rf: %f, bins: %d, dr: %f\n", r0, rf, BinsNumber, BinWidth);

  __InitializeBins();

  for (uint32_t i = 0; i < SavedSteps; i++) {
    __ComputeDistances(i);
    __CountDistances(i);
    __NormalizeCounts(i);
  }
}

void Correlation::ComputeHistogram(double r0, double rf, double dr)
{
  BinsNumber = (rf-r0)/dr;
  BinWidth = dr;

  ComputeHistogram(r0, rf, uint32_t(0));
}

void Correlation::LoadSpheres(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  Dimensions = Root["Dimensions"].asUInt();
  SpheresNumber = Root["SpheresNumber"].asUInt();
  SphereSize = Root["SphereSize"].asDouble();
  SavedSteps = Root["SavedSteps"].asUInt();

  // N_steps*M*(M-1)/2 because of symmetric distances and self-distance = 0
  DistancesNumber = SpheresNumber * (SpheresNumber - 1) * 0.5;
  if (Distances) delete[] Distances;
  Distances = new double[SavedSteps * DistancesNumber];

  if (Spheres) delete[] Spheres;
  Spheres = new Point[SavedSteps * SpheresNumber];

  if (SystemSize) delete[] SystemSize;
  SystemSize = new Point[2 * SavedSteps];

  if (SystemSizeHalf) delete[] SystemSizeHalf;
  SystemSizeHalf = new Point[SavedSteps];

  if (BoxParticleDensity) delete[] BoxParticleDensity;
  BoxParticleDensity = new double[SavedSteps];

  double V;
  for (uint32_t i = 0; i < SavedSteps; i++) {

    for (uint32_t j = 0; j < SpheresNumber; j++) {
      if (j < 2) SystemSize[2 * i + j].Init(Dimensions);
      Spheres[i * SpheresNumber + j].Init(Dimensions);
      for (uint32_t k = 0; k < Dimensions; k++) {
        if (j < 2) SystemSize[2 * i + j][k] = Root["SystemSize"][i][j][k].asDouble();
        Spheres[i * SpheresNumber + j][k] = Root["Data"][i][j][k].asDouble();
      }
    }

    SystemSizeHalf[i].Init(Dimensions);
    SystemSizeHalf[i] = (SystemSize[2 * i + 1] - SystemSize[2 * i]) * 0.5;

    V = 1.0;
    for (uint32_t k = 0; k < Dimensions; k++) {
      V *= SystemSize[2 * i + 1][k] - SystemSize[2 * i][k];
    }

    BoxParticleDensity[i] = SpheresNumber / V;
  }
}

void Correlation::SaveHistogram(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = Dimensions;
  Root["SpheresNumber"] = SpheresNumber;
  Root["SphereSize"] = SphereSize;
  Root["SavedSteps"] = SavedSteps;
  Root["FileType"] = "DensityHistogram";
  Root["BinWidth"] = BinWidth;
  Root["IDistance"] = IDistance;
  Root["FDistance"] = FDistance;
  Root["DistancesNumber"] = DistancesNumber;
  Root["BinsNumber"] = BinsNumber;

  for (uint32_t j = 0; j < BinsNumber; j++) {
    Root["BinDistances"][j] = BinDistances[j];
    for (uint32_t i = 0; i < SavedSteps; i++) {
      Root["NormalizedDensity"][i][j] = NormalizedDensity[i * BinsNumber + j];
<<<<<<< HEAD
      if (j == 0) Root["BoxParticleDensity"][i] = BoxParticleDensity[i];
=======
>>>>>>> 8ebc1ce7c21bb664fa948fa5f207337baaed4f64
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
