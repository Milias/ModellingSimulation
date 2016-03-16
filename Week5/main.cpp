#include "correlation.h"

int main(int argc, char ** argv)
{
  if(argc > 1) {
    if (strcmp(argv[1],"-dhist") == 0 && argc == 7) {
      Correlation corr;
      corr.LoadSpheres(argv[2]);
      corr.ComputeHistogram(std::atof(argv[3]), std::atof(argv[4]), std::atof(argv[5]));
      corr.SaveHistogram(argv[6]);
    } else if (strcmp(argv[1],"-bhist") == 0 && argc == 7) {
      Correlation corr;
      corr.LoadSpheres(argv[2]);
      corr.ComputeHistogram(std::atof(argv[3]), std::atof(argv[4]), uint32_t(std::atol(argv[5])));
      corr.SaveHistogram(argv[6]);
    }
  }
  return 0;
}
