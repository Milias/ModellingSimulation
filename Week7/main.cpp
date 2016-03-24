#include "lennardjones.hpp"

int main(int argc, char ** argv)
{
  LennardJones<3> lds;
  if (argc > 1) {
    if (strcmp(argv[1], "-evolve") == 0 && argc == 5) {
      lds.InitializeFromFile(argv[2]);
      lds.LoadParticles(argv[3]);
      lds.UpdateParticles();
      lds.SaveParticles(argv[4]);
    } else {
      std::cout << "Error: wrong arguments.\n";
    }
  } else {
    std::cout << "No arguments.\n";
  }
  return 0;
}
