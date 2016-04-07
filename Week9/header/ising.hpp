#pragma once
#include "shared.h"

template <typename T, uint32_t D> struct SystemArray
{
  T * Array = nullptr;

  uint32_t
    * Size = nullptr,
    TotalSize = 0;

  SystemArray(uint32_t * dim) {
    Size = new uint32_t[D];
    TotalSize = 1;
    for (uint32_t i = 0; i < D; i++) {
      TotalSize *= dim[i];
      Size[i] = dim[i];
    }
    Array = new T[TotalSize];
  }

  uint32_t GetIndex(uint32_t * loc) {
    uint32_t ind = loc[D-1] % Size[D-1];
    for (uint32_t i = 2; i <= D; i++) {
      ind *= Size[D-i];
      ind += loc[D-i] % Size[D-i];
    }
    return ind;
  }

  void SetGlobal(T a) {
    for (uint32_t i = 0; i < TotalSystemSize; i++) { Array[i] = a; }
  }

  void SetLocal(T a, uint32_t i) { Array[i] = a; }

  void SetLocal(T a, uint32_t * loc) {
    Array[GetIndex(loc)] = a;
  }

  T GetLocal(uint32_t i) { return Array[i]; }
  T GetLocal(uint32_t * i) { return Array[GetIndex(i)]; }

  T * Data() const { return Array; }
};

template <uint32_t D> class IsingModel
{
protected:
  bool * SystemArray = nullptr;

  double
    * Probabilities,
    Beta = 0.0,
    B = 0.0;

  uint32_t
    * SystemSize = nullptr,
    TotalSystemSize = 0,
    TotalSteps = 0,
    CurrentStep = 0;

  RandomGenerator Random;

  bool __FlipSpin();

public:
  IsingModel();
  ~IsingModel();
};

template <uint32_t D> IsingModel<D>::IsingModel()
{
  Probabilities = new double[2*D+1];
}

template <uint32_t D> IsingModel<D>::~IsingModel()
{
  delete[] SystemArray;
  delete[] Probabilities;
  delete[] SystemSize;
}

template <uint32_t D> bool IsingModel<D>::__FlipSpin()
{
  uint32_t ind = Random.RandomInt();
  
}
