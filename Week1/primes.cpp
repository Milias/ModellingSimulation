#include "primes.h"

void seqSieve(uint64_t bound, std::vector<uint64_t> &primes)
{
  uint64_t b2 = bound/2-1; bound = sqrt(1.0*bound);
  bool *c; c = new bool[b2];
  uint64_t s = 3; uint64_t si = 0;

  for(uint64_t i = s; s <= bound; i++) {
    for(uint64_t j = si+(si+1)*s; j < b2; j+=s) {
      if(c[j]) { continue; }
      c[j] = true;
    }
    for(uint64_t j = si+1; j < b2; j++) {
      if(!c[j]) { s = 2*j+3; si = j; break; }
    }
  }

  primes.push_back(2);
  for (uint64_t i = 0; i < b2; i++) {
    if (!c[i]) {
      primes.push_back(2*i+3);
    }
  }

  delete[] c;
}
