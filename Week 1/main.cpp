#include <iostream>
#include "primes.h"

int main(int argc, char ** argv)
{
  DefaultClock gc;
  double T = 0.0;

  std::vector<uint64_t> primes;
  uint64_t bound = 0;
  bool print = false;
  if (argc >= 2) bound = pow(2,atoi(argv[1]));
  if (argc >= 3) print = strcmp(argv[2],"print")==0;

  t_point t0 = gc.now();
  for (uint64_t i = bound; primes.size() < bound; i*=3) {
    primes.clear();
    seqSieve(i, primes);
  }
  T = std::chrono::duration<double>(gc.now() - t0).count();

  for (uint64_t i = 0; i < bound && print; i++) {
    std::cout << primes[i] << std::endl;
  }

  printf("N: %d, elapsed: %2.7fs\n",bound,T);
  return 0;
}
