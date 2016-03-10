#pragma once

//STL
#include <cstdint>
#include <cmath>
#include <cstring>
#include <chrono>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <cstdio>

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

// ThirdPary
#include "json.h"

typedef std::chrono::high_resolution_clock DefaultClock;
typedef std::chrono::time_point<DefaultClock> t_point;

const double pi = 3.14159265359;

/*
  This structure defines an N-dimensional
  point in space, alongside a variety of
  convenience methods.
*/
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

  void Free() {
    if (Values) delete[] Values;
    N = 0;
  }

  void Free(uint32_t dim) {
    if (Values) delete[] Values;
    N = dim;
    Values = new double[N];
  }

  Point & Init(uint32_t dim, double a) {
    N = dim;
    Values = new double[N];
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = a;
    }
    return *this;
  }

  Point & Init(uint32_t dim) {
    N = dim;
    Values = new double[N];
    return *this;
  }

  Point & operator=(const Point & p) {
    assert(N > 0 && Values != NULL && p.N > 0 && p.Values != NULL);
    if (p.N != N) {
      N = p.N;
      delete[] Values;
      Values = new double[N];
    }
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
    return *this;
  }

  Point & operator=(double a) {
    assert(N > 0 && Values != NULL);
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = a;
    }
    return *this;
  }

  Point & operator=(double * p) {
    assert(N > 0 && Values != NULL && p != NULL);
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
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

  Point operator-() {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = -Values[i];
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

  /*
    Modulus operator, used to implement periodic
    boundary conditions in the random walk.

    Small steps compared to "a" are assumed.
  */
  Point operator%(double a) {
    Point result(*this);
    result %= a;
    return result;
  }

  Point & operator%=(double a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = (Values[i] > a ? Values[i]-(1+std::trunc(Values[i]/a))*a : (Values[i] < -a ? (1+std::trunc(-Values[i]/a))*a+Values[i] : Values[i]));
    }
    return *this;
  }

  Point operator%(const Point & p) {
    Point result(*this);
    result %= p;
    return result;
  }

  Point & operator%=(const Point & p) {
    double t;
    for (uint32_t i = 0; i < N; i++) {
      t = 2 * p[i];
      if (Values[i] > p[i]) {
        Values[i] = - p[i] + std::fmod(std::fabs(p[i] - Values[i]),t);
      } else if (Values[i] < -p[i]) {
        Values[i] = p[i] - std::fmod(std::fabs(p[i] + Values[i]),t);
      }
    }
    return *this;
  }

  /*
    Periodic boundary conditions.
      - lp: left point.
      - rp: right point.
  */
  Point & Wrap(const Point & lp, const Point & rp) {
    double t;
    for (uint32_t i = 0; i < N; i++) {
      t = rp[i] - lp[i];
      if (Values[i] > rp[i]) {
        Values[i] = lp[i] + std::fmod(std::fabs(rp[i] - Values[i]),t);
      } else if (Values[i] < lp[i]) {
        Values[i] = rp[i] - std::fmod(std::fabs(lp[i] - Values[i]),t);
      }
    }
    return *this;
  }

  Point & Wrap(const Point & rp) {
    for (uint32_t i = 0; i < N; i++) {
      if (Values[i] > rp[i]) {
        Values[i] = std::fmod(std::fabs(rp[i] - Values[i]),rp[i]);
      } else if (Values[i] < 0) {
        Values[i] = rp[i] - std::fmod(std::fabs(Values[i]),rp[i]);
      }
    }
    return *this;
  }
};
