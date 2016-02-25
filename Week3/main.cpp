#include "hardspheres.h"

int main(int argc, char ** argv)
{
  if (argc == 1) {
    printf("Usage:\n");
  } else if(argc > 1) {
    if (strcmp(argv[1],"-rect") == 0 && argc == 6) {
      printf("Generating rectangular lattice. Dim = %d, SphPerDim = %d, Size = %1.2f\n", std::atoi(argv[2]), std::atoi(argv[3]), std::atof(argv[4]));
      HardSpheres hs;
      hs.GenerateRectangular(std::atoll(argv[2]), std::atoll(argv[3]), std::atof(argv[4]));
      hs.SaveSpheres(argv[5]);
    } else if (strcmp(argv[1],"-rect") == 0 && argc == 7) {
      printf("Generating rectangular lattice. Dim = %d, SphPerDim = %d, Size = %1.2f, Scaling = %1.2f\n", std::atoi(argv[2]), std::atoi(argv[3]), std::atof(argv[4]), std::atof(argv[5]));
      HardSpheres hs;
      hs.GenerateRectangular(std::atoll(argv[2]), std::atoll(argv[3]), std::atof(argv[4]), std::atof(argv[5]));
      hs.SaveSpheres(argv[6]);
    } else if (strcmp(argv[1],"-hex") == 0 && argc == 5) {
      printf("Generating hexagonal lattice. SphPerDim = %d, Size = %1.2f\n", std::atoi(argv[2]), std::atof(argv[3]));
      HardSpheres hs;
      hs.GenerateHexagonal(std::atoll(argv[2]), std::atof(argv[3]));
      hs.SaveSpheres(argv[4]);
    } else if (strcmp(argv[1],"-hex") == 0 && argc == 6) {
      printf("Generating hexagonal lattice. SphPerDim = %d, Size = %1.2f, Scaling = %1.2f\n", std::atoi(argv[2]), std::atof(argv[3]), std::atof(argv[4]));
      HardSpheres hs;
      hs.GenerateHexagonal(std::atoll(argv[2]), std::atof(argv[3]), std::atof(argv[4]));
      hs.SaveSpheres(argv[5]);
    } else {
      printf("Wrong combination of arguments.\n");
    }
  }

  return 0;
}
