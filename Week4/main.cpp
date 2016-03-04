#include "hardspheres.h"

int main(int argc, char ** argv)
{
  if (argc == 1) {
    printf("Usage:\n");
  } else if(argc > 1) {
    if (strcmp(argv[1],"-gen") == 0 && argc == 4) {
      printf("Generating lattice from file %s.\n", argv[2]);
      HardSpheres hs;
      hs.GenerateFromFile(argv[2]);
      hs.SaveSpheres(argv[3]);

    } else if (strcmp(argv[1],"-evolve") == 0 && argc == 8) {
      printf("Evolving %s, Steps: %d.\n", argv[5], std::atoi(argv[3]));
      HardSpheres hs;
      hs.LoadSpheres(argv[5]);
      hs.UpdateParticles(std::atof(argv[2]), std::atoll(argv[3]), std::atoll(argv[4]), std::atoll(argv[5]));
      hs.SaveStoredSpheres(argv[6]);
    } else {
      printf("Wrong argument combination.\n");
    }
  }

  return 0;
}
