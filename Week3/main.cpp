#include "hardspheres.h"

int main(int argc, char ** argv)
{
  if (argc == 1) {
    printf("Usage:\n");
  } else if(argc > 1) {
    if (strcmp(argv[1],"-rect") == 0 && argc == 7) {
      printf("Generating rectangular lattice. Dim = %d, SphPerDim = %d, Size = %1.2f, Scaling = %1.2f\n", std::atoi(argv[2]), std::atoi(argv[3]), std::atof(argv[4]), std::atof(argv[5]));
      HardSpheres hs;
      hs.GenerateRectangular(std::atoll(argv[2]), std::atoll(argv[3]), std::atof(argv[4]), std::atof(argv[5]));
      hs.SaveSpheres(argv[6]);

    } else if (strcmp(argv[1],"-hex") == 0 && argc == 6) {
      printf("Generating hexagonal lattice. SphPerDim = %d, Size = %1.2f, Scaling = %1.2f\n", std::atoi(argv[2]), std::atof(argv[3]), std::atof(argv[4]));
      HardSpheres hs;
      hs.GenerateHexagonal(std::atoll(argv[2]), std::atof(argv[3]), std::atof(argv[4]));
      hs.SaveSpheres(argv[5]);

    } else if (strcmp(argv[1],"-gen") == 0 && argc == 4) {
      printf("Generating lattice from file %s.\n", argv[2]);
      HardSpheres hs;
      hs.GenerateFromFile(argv[2]);
      hs.SaveSpheres(argv[3]);

    } else if (strcmp(argv[1],"-evolve") == 0 && argc == 7) {
      printf("Evolving %s, Steps: %d.\n", argv[5], std::atoi(argv[3]));
      HardSpheres hs;
      hs.LoadSpheres(argv[5]);
      hs.UpdateParticles(std::atof(argv[2]), std::atoll(argv[3]), std::atoll(argv[4]));
      hs.SaveStoredSpheres(argv[6]);

    } else {
      printf("Wrong combination of arguments.\n");
    }
  }

  return 0;
}
