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

  ~SystemArray() {
    delete[] Array;
    delete[] Size;
  }

  uint32_t GetIndex(uint32_t * loc) {
    uint32_t ind = loc[D-1];
    for (uint32_t i = 2; i <= D; i++) {
      ind *= Size[D-i];
      ind += loc[D-i];
    }
    return ind;
  }

  void SetGlobal(T a) {
    for (uint32_t i = 0; i < TotalSize; i++) { Array[i] = a; }
  }

  void SetLocal(T a, uint32_t i) { Array[i] = a; }
  void SetLocal(T a, uint32_t * loc) { Array[GetIndex(loc)] = a; }

  T GetLocal(uint32_t i) { return Array[i]; }
  T GetLocal(uint32_t * i) { return Array[GetIndex(i)]; }

  T * Data() const { return Array; }
};

template <uint32_t D> class IsingModel
{
protected:
  SystemArray<bool, D> * System = nullptr;

  bool Initialize = false;

  double
    * Probabilities = nullptr,
    Beta = 0.0,
    J = 0.0;

  uint32_t
    * TempLoc = nullptr,
    TotalSteps = 0,
    CurrentStep = 0;

  RandomGenerator * Random;

  bool __FlipSpin();

public:
  IsingModel();
  ~IsingModel();

  virtual void InitializeFromFile(char const * filename);
};

template <uint32_t D> IsingModel<D>::IsingModel() : Probabilities(new double[D]), TempLoc(new uint32_t[D]), Random(new RandomGenerator[D]) {}

template <uint32_t D> IsingModel<D>::~IsingModel()
{
  delete[] TempLoc;
  delete[] Probabilities;
  delete[] Random;
  if (System) delete System;
}

template <uint32_t D> bool IsingModel<D>::__FlipSpin()
{
  // true : spin up (+), false: spin down (-)
  int32_t spin_sum = 0;
  double acc = 1.0;

  for (uint32_t i = 0; i < D; i++) { TempLoc[i] = Random[i].RandomInt(); }

  uint32_t ind = System->GetIndex(TempLoc);

  for (uint32_t i = 0; i < D; i++) {
    TempLoc[i] = TempLoc[i] + 1 == System->Size[i] ? 0 : TempLoc[i] + 1;
    spin_sum += System->GetLocal(TempLoc) ? 1 : -1;
    TempLoc[i] = TempLoc[i] - 1 == 0 ? System->Size[i] - 1 : TempLoc[i] - 1;
    TempLoc[i] = TempLoc[i] - 1 == 0 ? System->Size[i] - 1 : TempLoc[i] - 1;
    spin_sum += System->GetLocal(TempLoc) ? 1 : -1;
    TempLoc[i] = TempLoc[i] + 1 == System->Size[i] ? 0 : TempLoc[i] + 1;
  }

  if (System->GetLocal(ind)) {
    if (spin_sum > 0) {
      acc = Probabilities[spin_sum];
    }
  } else {
    if (spin_sum < 0) {
      acc = Probabilities[-spin_sum];
    }
  }

  if (acc < 1.0) {
    if (acc < Random[0].RandomProb()) {
      System->SetLocal(!System->GetLocal(ind),ind);
      return true;
    }
  } else {
    System->SetLocal(!System->GetLocal(ind),ind);
    return true;
  }

  return false;
}

template <uint32_t D> void IsingModel<D>::InitializeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  Beta = Root["Beta"];
  J = Root["J"];

  for (uint32_t i = 1; i <= D; i++) {
    Probabilities[i] = std::exp(-Beta*4*J*i);
  }

  for (uint32_t i = 0; i < D; i++) {
    Random[i].SetRange(0, System->Size[i] - 1);
  }

  Initialize = true;
}
