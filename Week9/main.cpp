#include "ising.hpp"

int main(int argc, char ** argv)
{
  IsingModel<2> im2;
  IsingModel<3> im3;
  if (argc > 1) {
    if (strcmp(argv[1], "--ising2") == 0 && argc == 5) {
      im2.InitializeFromFile(argv[2]);
      im2.LoadSystem(argv[3]);
      im2.UpdateSystem();
      im2.SaveSystem(argv[4]);
    } else if (strcmp(argv[1], "--ising3") == 0 && argc == 5) {
      im3.InitializeFromFile(argv[2]);
      im3.LoadSystem(argv[3]);
      im3.UpdateSystem();
      im3.SaveSystem(argv[4]);
    } else {
      std::cout << "Error: wrong arguments.\n";
    }
  } else {
    std::cout << "No arguments.\n";
  }
  return 0;
}
