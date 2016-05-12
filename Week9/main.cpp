#include "ising.hpp"

int main(int argc, char ** argv)
{
  IsingModel<3> im;
  if (argc > 1) {
    if (strcmp(argv[1], "--ising") == 0 && argc == 5) {
      im.InitializeFromFile(argv[2]);
      im.LoadSystem(argv[3]);
      im.UpdateSystem();
      im.SaveSystem(argv[4]);
    } else {
      std::cout << "Error: wrong arguments.\n";
    }
  } else {
    std::cout << "No arguments.\n";
  }
  return 0;
}
