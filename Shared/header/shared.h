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

// ThirdPary
#include "json.h"

typedef std::chrono::high_resolution_clock DefaultClock;
typedef std::chrono::time_point<DefaultClock> t_point;

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

  /*
    Modulus operator, used to implement periodic
    boundary conditions in the random walk.

    Small steps compared to "a" are assumed.
  */
  Point operator%(double a) {
    Point result(N);
    for (uint32_t i = 0; i < N; i++) {
      result[i] = (Values[i] > a ? -2*a+Values[i] : (Values[i] < -a ? 2*a+Values[i] : Values[i]));
    }
    return result;
  }

  Point & operator%=(double a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = (Values[i] > a ? -2*a+Values[i] : (Values[i] < -a ? 2*a+Values[i] : Values[i]));
    }
    return *this;
  }
};
