#pragma once
#include "shared.h"

struct Point
{
  uint32_t N = 0;
  double * Values = NULL;

  Point() : N(0), Values(NULL) {}

  Point(uint32_t dim) : N(dim) {
    Values = new double[N];
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = 0.0;
    }
  }

  double & operator[](uint32_t i) const { return Values[i]; }

  Point(uint32_t dim, double * p) : N(dim) {
    Values = new double[N];
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
  }

  Point(const Point & p) : N(p.N) {
    Values = new double[N];
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
  }

  ~Point() {
    if (Values) delete[] Values;
  }

  Point & operator=(const Point & p) {
    N = p.N;
    if (Values == NULL) { Values = new double[N]; }
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
    return *this;
  }

  Point & operator=(double a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = a;
    }
    return *this;
  }

  Point operator+(const Point & p) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = Values[i] + p[i];
    }
    return result;
  }

  Point operator-(const Point & p) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = Values[i] - p[i];
    }
    return result;
  }

  Point & operator+=(const Point & p) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] += p[i];
    }
    return *this;
  }

  Point & operator-=(const Point & p) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] -= p[i];
    }
    return *this;
  }

  Point operator+(double a) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = Values[i] + a;
    }
    return result;
  }

  Point operator-(double a) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = Values[i] - a;
    }
    return result;
  }

  Point & operator+=(double a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] += a;
    }
    return *this;
  }

  Point & operator-=(double a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] -= a;
    }
    return *this;
  }

  Point operator*(double a) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = Values[i] * a;
    }
    return result;
  }

  Point operator/(double a) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = Values[i] / a;
    }
    return result;
  }

  Point & operator*=(double a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] *= a;
    }
    return *this;
  }

  Point & operator/=(double a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] /= a;
    }
    return *this;
  }

  double operator*(const Point & p) {
    double s = 0.0;
    for (uint32_t i = 0; i < N; i++) {
      s += Values[i]*p[i];
    }
    return s;
  }

  Point operator%(double a) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = fmod(Values[i],a);
    }
    return result;
  }
};

class RandomWalk
{
private:
  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealdistribution;

  std::function<double()> RandomDouble;

  double StepLength = 0.0, BoxSize = 0.0;
  uint32_t Dimensions = 0, StepNumber = 0, WalksNumber = 0;

  Point * StepLocations = NULL;
  double * RSquared = NULL;

public:
  RandomWalk(uint32_t dim);
  ~RandomWalk();

  Point RandomUnitaryVector();
  Point * SimpleRandomWalk(double delta, uint32_t steps);
  double * SimpleComputeRSquared(double delta, uint32_t steps, uint32_t nwalks);
  Point * PeriodicRandomWalk(double delta, uint32_t steps, double size);
  double * PeriodicComputeRSquared(double delta, uint32_t steps, uint32_t nwalks, double size);

  void SaveWalkData(char const * filename);
  void SaveRData(char const * filename);
};
