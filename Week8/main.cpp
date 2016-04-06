#include "ozsolver.hpp"

int main(int argc, char ** argv)
{
  if (argc > 1) {
    if (strcmp(argv[1], "-muvt") == 0 && argc == 5) {
    } else {
      std::cout << "Error: wrong arguments.\n";
    }
  } else {
    std::cout << "No arguments.\n";
  }
  return 0;
}
