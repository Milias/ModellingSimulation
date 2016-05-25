#include "elevator.hpp"

int main(int argc, char ** argv)
{
  Elevator ele;
  if (argc > 1) {
    if (strcmp(argv[1], "--elevator") == 0 && argc == 5) {
      ele.InitializeFromFile(argv[2]);
      ele.LoadSystem(argv[3]);
      ele.Integrate();
      ele.SaveSystem(argv[4]);
    } else {
      std::cout << "Error: wrong arguments.\n";
    }
  } else {
    std::cout << "No arguments.\n";
  }
  return 0;
}
