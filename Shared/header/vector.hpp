#pragma once
#include <cmath>

template <uint32_t N, typename T> struct GenericVector
{
  T * Values;

  static GenericVector<N, T> Zero() { return GenericVector<N, T>(0.0); }

  GenericVector<N, T>() : Values(new T[N]) {}

  GenericVector<N, T>(T a) : Values(new T[N]) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = a;
    }
  }

  GenericVector<N, T>(T * p) : Values(new T[N]) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
  }

  GenericVector<N, T>(const GenericVector<N, T> & p) : Values(new T[N]) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
  }

  ~GenericVector<N, T>() { delete[] Values; }

  T & operator[](uint32_t i) const { return Values[i]; }

  GenericVector<N, T> & operator=(const GenericVector<N, T> & p) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
    return *this;
  }

  GenericVector<N, T> & operator=(T a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = a;
    }
    return *this;
  }

  GenericVector<N, T> & operator=(T * p) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = p[i];
    }
    return *this;
  }

  GenericVector<N, T> operator+(const GenericVector<N, T> & p) {
    return GenericVector<N, T>(*this) += p;
  }

  GenericVector<N, T> operator-(const GenericVector<N, T> & p) {
    return GenericVector<N, T>(*this) -= p;
  }

  GenericVector<N, T> & operator+=(const GenericVector<N, T> & p) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] += p[i];
    }
    return *this;
  }

  GenericVector<N, T> & operator-=(const GenericVector<N, T> & p) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] -= p[i];
    }
    return *this;
  }

  GenericVector<N, T> operator+(T a) {
    return GenericVector<N, T>(*this) += a;
  }

  GenericVector<N, T> operator-(T a) {
    return GenericVector<N, T>(*this) -= a;
  }

  GenericVector<N, T> operator-() {
    GenericVector<N, T> result;
    for (uint32_t i = 0; i < N; i++) {
      result[i] = -Values[i];
    }
    return result;
  }

  GenericVector<N, T> & operator+=(T a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] += a;
    }
    return *this;
  }

  GenericVector<N, T> & operator-=(T a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] -= a;
    }
    return *this;
  }

  GenericVector<N, T> operator*(T a) {
    return GenericVector<N, T>(*this) *= a;
  }

  GenericVector<N, T> operator/(T a) {
    return GenericVector<N, T>(*this) /= a;
  }

  GenericVector<N, T> & operator*=(T a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] *= a;
    }
    return *this;
  }

  GenericVector<N, T> & operator/=(T a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] /= a;
    }
    return *this;
  }

  T operator*(const GenericVector<N, T> & p) {
    T s = 0.0;
    for (uint32_t i = 0; i < N; i++) {
      s += Values[i]*p[i];
    }
    return s;
  }

  /*
    Modulus operator, used to implement periodic
    boundary conditions in the random walk.
  */

  GenericVector<N, T> operator%(const GenericVector<N, T> & p) {
    return GenericVector<N, T>(*this) %= p;
  }

  GenericVector<N, T> operator%(T a) {
    return GenericVector<N, T>(*this) %= a;
  }

  GenericVector<N, T> & operator%=(T a) {
    for (uint32_t i = 0; i < N; i++) {
      Values[i] = (Values[i] > a ? Values[i]-(1+std::trunc(Values[i]/a))*a : (Values[i] < -a ? (1+std::trunc(-Values[i]/a))*a+Values[i] : Values[i]));
    }
    return *this;
  }

  GenericVector<N, T> & operator%=(const GenericVector<N, T> & p) {
    T t;
    for (uint32_t i = 0; i < N; i++) {
      t = 2 * p[i];
      if (Values[i] > p[i]) {
        Values[i] = - p[i] + std::fmod(std::fabs(p[i] - Values[i]),t);
      } else if (Values[i] < -p[i]) {
        Values[i] = p[i] - std::fmod(std::fabs(- p[i] - Values[i]),t);
      }
    }
    return *this;
  }

  /*
    Periodic boundary conditions.
      - lp: left GenericVector<N, T>.
      - rp: right GenericVector<N, T>.
  */
  GenericVector<N, T> & Wrap(const GenericVector<N, T> & lp, const GenericVector<N, T> & rp) {
    T t;
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

  GenericVector<N, T> & Wrap(const GenericVector<N, T> & rp) {
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

template <uint32_t N, typename T> GenericVector<N, T> operator+(T a, const GenericVector<N, T> & p) {
  return p + a;
}

template <uint32_t N, typename T> GenericVector<N, T> operator-(T a, const GenericVector<N, T> & p) {
  return -p + a;
}

template <uint32_t N, typename T> GenericVector<N, T> operator*(T a, const GenericVector<N, T> & p) {
  return p * a;
}

template <uint32_t N, typename T> GenericVector<N, T> operator/(T a, const GenericVector<N, T> & p) {
  GenericVector<N, T> result;
  for (uint32_t i = 0; i < N; i++) {
    result[i] = a / p[i];
  }
  return result;
}

template <uint32_t N> using Vector = GenericVector<N, double>;
