#include "hitmiss.h"
#include "randomwalk.h"

int main(int argc, char ** argv)
{
  double realPi = 3.14159265359;

  if (argc == 1) {
    printf("Usage:\n-N n\tComputes Pi using n points.\n-e e\tComputes Pi with maximum error e.\n-walk dim N file\tSimple random walk with dimensions dim, N steps and saves it to file in json format.\n-rsquared dim N n_walks file\tComputes <R^2(N)> with dimensions dim, N steps, n_walks walks and saves it to file in json format.\n-pwalk dim N size file\n-prsquared dim N n_walks size file\tVersion with periodic boundary conditions, an extra argument before file indicates the size of the box.\n");
  } else if (argc == 3) {
    if (strcmp(argv[1],"-N") == 0) {
      HitMiss hm;
      double pi = hm.Simulate(static_cast<uint32_t>(std::atoll(argv[2])));
      printf("Pi = %1.10f, error = %e\n",pi,std::abs(pi-realPi));

    } else if (strcmp(argv[1],"-e") == 0) {
      HitMiss hm;
      double pi = hm.Simulate(static_cast<double>(std::atof(argv[2])));
      printf("Pi = %1.10f, error = %e\n",pi,std::abs(pi-realPi));
    }

  } else if (argc == 5) {
    if (strcmp(argv[1],"-walk") == 0) {
      RandomWalk rw(std::atoll(argv[2]));
      rw.ComputeRandomWalk(std::atoll(argv[3]));
      rw.SaveWalkData(argv[4]);
    }

  } else if (argc == 6) {
    if (strcmp(argv[1],"-rsquared") == 0) {
      RandomWalk rw(std::atoll(argv[2]));
      rw.ComputeRSquared(std::atoll(argv[3]), std::atoll(argv[4]));
      rw.SaveRData(argv[5]);

    } else if (strcmp(argv[1],"-pwalk") == 0) {
      RandomWalk rw(std::atoll(argv[2]));
      rw.ComputeRandomWalk(std::atoll(argv[3]), std::atof(argv[4]));
      rw.SaveWalkData(argv[5]);
    }

  } else if (argc == 7) {
    if (strcmp(argv[1],"-prsquared") == 0) {
       RandomWalk rw(std::atoll(argv[2]));
       rw.ComputeRSquared(std::atoll(argv[3]), std::atoll(argv[4]), std::atof(argv[5]));
       rw.SaveRData(argv[6]);
     }
  }
}
