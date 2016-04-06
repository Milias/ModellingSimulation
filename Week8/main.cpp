#include "ozsolver.hpp"

int main(int argc, char ** argv)
{
  if (argc > 1) {
    if (strcmp(argv[1], "-oz") == 0 && argc == 7) {
        OZSolverHardSpheres ozs(std::atof(argv[2]),std::atof(argv[3]),1 << std::atoi(argv[4]),std::atof(argv[5]));
        ozs.ComputeOutputAndSave(argv[6]);
    } else {
      std::cout << "Error: wrong arguments.\n";
    }
  } else {
    std::cout << "No arguments.\n";
  }
  return 0;
}
