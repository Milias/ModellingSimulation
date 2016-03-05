#include "hardspheres.h"

int main(int argc, char ** argv)
{
  if(argc > 1) {
    if (strcmp(argv[1],"-gevolve") == 0 && argc == 5) {
      HardSpheres hs;
      std::cout << argv[2] << std::endl;
      std::cout << argv[3] << std::endl;
      std::cout << argv[4] << std::endl;
      hs.InitializeFromFile(argv[2]);
      hs.GenerateLatticeFromFile(argv[3]);
      hs.UpdateParticles();
      hs.SaveSpheres(argv[4]);
    } else {
      printf("Wrong argument combination.\n");
    }
  }

  return 0;
}
