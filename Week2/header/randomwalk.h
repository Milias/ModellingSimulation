#pragma once
#include "shared.h"

/*
  Tbis class defines several methods used to compute
  random walks, as well as <R^2(N)> and saving results
  to disk.

  Among the attributes we can find a random number
  generator from the standard library (usually an
  implementation of the mt19937 algorithm), several
  information variables and two C arrays of size
  StepNumber to store R(N) and <R^2(N)>.

  Random walks can be pèrformed with or without periodic
  boundary conditions, by setting a BoxSize larger
  than zero.
*/
class RandomWalk
{
private:
  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealDistribution;

  std::function<double()> RandomDouble;

  double BoxSize = 0.0;
  uint32_t Dimensions = 0, StepNumber = 0, WalksNumber = 0;

  //Point is defined in "shared.h"
  Point * StepLocations = NULL;
  double * RSquared = NULL;

public:
  /*
    Within this constructor the random number generator
    is initialized, and problem's dimension chosen.
  */
  RandomWalk(uint32_t dim);
  ~RandomWalk();

  /*
    This function either takes a Point or returns one,
    with its components set to a location in the unit
    sphere of the right dimensions.
  */
  Point RandomUnitaryVector();
  Point & RandomUnitaryVector(Point & p);

  /*
    This method computes a random walk of "step" steps,
    and optionally within a N-dimensional cube of size
    "size" with periodic boundary conditions.

    If "size" is equal to zero, there will be no boundary
    conditions.

    The result, a pointer to the first element of an array
    of Points, is both stored in "StepLocations" and returned.
  */
  Point * ComputeRandomWalk(uint32_t steps, double size = 0.0);

  /*
    In this method the average distance after each step
    of a random walk is computed, by averaging over "nwalks"
    walks.

    In the implementation ComputeRandomWalk(uint32_t, double)
    is called that many times, and each value of R^2(N) summed
    in the corresponding location in "RSquared", so it can be
    averaged over.

    Finally a pointer to this array is returned.
  */
  double * ComputeRSquared(uint32_t steps, uint32_t nwalks, double size = 0.0);

  /*
    These two methods are used to save to file values stored
    either in "StepLocations" or "RSquared" in Json format.

    This is done by making use of the library jsoncpp, included
    in "shared.h".
  */
  void SaveWalkData(char const * filename);
  void SaveRData(char const * filename);
};
