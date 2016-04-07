#include "ising.hpp"

int main(int argc, char ** argv)
{
  if (argc > 1) {
    if (strcmp(argv[1], "-oz") == 0 && argc == 7) {
    } else {
      std::cout << "Error: wrong arguments.\n";
    }
  } else {
    std::cout << "No arguments.\n";
  }
  return 0;
}
