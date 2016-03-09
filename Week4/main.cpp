#include "hardspheres.h"

int main(int argc, char ** argv)
{
  if(argc > 1) {
    if (strcmp(argv[1],"-gevolve") == 0 && argc == 5) {
      HardSpheres hs;
      hs.InitializeFromFile(argv[2]);
      hs.GenerateLatticeFromFile(argv[3]);
      hs.UpdateParticles();
      hs.SaveSpheres(argv[4]);
    } else if (strcmp(argv[1],"-levolve") == 0 && argc == 5) {
      HardSpheres hs;
      hs.InitializeFromFile(argv[2]);
      hs.LoadSpheres(argv[3]);
      hs.UpdateParticles();
      hs.SaveSpheres(argv[4]);
    } else {
      printf("Wrong argument combination.\n");
    }
  }

  return 0;
}
