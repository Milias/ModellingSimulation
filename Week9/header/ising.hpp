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

  bool ** StoredSystem = nullptr;

  double
    * Probabilities = nullptr,
    Beta = 0.0,
    J = 0.0;

  uint32_t
    * TempLoc = nullptr,
    TotalSteps = 0,
    SaveInterval = 0,
    SavedSteps = 0,
    CurrentStep = 0;

  RandomGenerator * Random = nullptr;

  bool __FlipSpin();
  virtual void __Measure() {}
  void __SaveSystem(uint32_t step);

public:
  IsingModel();
  ~IsingModel();

  virtual void InitializeFromFile(char const * filename);
  void UpdateSystem();
  void LoadSystem(char const * filename);
  void SaveSystem(char const * filename);
};





template <uint32_t D> IsingModel<D>::IsingModel() :
  Probabilities(new double[D]),
  TempLoc(new uint32_t[D]),
  Random(new RandomGenerator[D]) {}

template <uint32_t D> IsingModel<D>::~IsingModel()
{
  delete[] TempLoc;
  delete[] Probabilities;
  delete[] Random;
  if (System) delete System;

  if (StoredSystem) {
    for (uint32_t i = 0; i < TotalSteps; i++) { delete[] StoredSystem[i]; }
    delete[] StoredSystem;
  }
}

template <uint32_t D> bool IsingModel<D>::__FlipSpin()
{
  // true : spin up (+), false: spin down (-)
  int32_t spin_sum = 0;
  double acc = 1.0;

  for (uint32_t i = 0; i < D; i++) {
    TempLoc[i] = Random[i].RandomInt();
  }

  uint32_t ind = System->GetIndex(TempLoc);

  for (uint32_t i = 0; i < D; i++) {
    TempLoc[i] = TempLoc[i] + 1 == System->Size[i] ? 0 : TempLoc[i] + 1;
    spin_sum += System->GetLocal(TempLoc) ? 1 : -1;
    TempLoc[i] = TempLoc[i] == 0 ? System->Size[i] - 1 : TempLoc[i] - 1;

    TempLoc[i] = TempLoc[i] == 0 ? System->Size[i] - 1 : TempLoc[i] - 1;
    spin_sum += System->GetLocal(TempLoc) ? 1 : -1;
    TempLoc[i] = TempLoc[i] + 1 == System->Size[i] ? 0 : TempLoc[i] + 1;
  }

  bool spin = System->GetLocal(ind);
  if (spin) {
    if (spin_sum > 0) {
      acc = Probabilities[spin_sum-1];
    }
  } else {
    if (spin_sum < 0) {
      acc = Probabilities[-spin_sum-1];
    }
  }

  if (acc < 1.0) {
    if (acc < Random[0].RandomProb()) {
      System->SetLocal(!spin,ind);
      return true;
    }
  } else {
    System->SetLocal(!spin,ind);
    return true;
  }

  return false;
}

template <uint32_t D> void IsingModel<D>::__SaveSystem(uint32_t step)
{
  for (uint32_t i = 0; i < System->TotalSize; i++) {
    StoredSystem[step][i] = System->GetLocal(i);
  }
}

template <uint32_t D> void IsingModel<D>::InitializeFromFile(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  Beta = Root["Beta"].asDouble();
  J = Root["J"].asDouble();
  TotalSteps = Root["TotalSteps"].asUInt();
  SaveInterval = Root["SaveInterval"].asUInt();
  SavedSteps = SaveInterval > 0 ? TotalSteps / SaveInterval + 1 : 0;

  for (uint32_t i = 1; i <= D; i++) {
    Probabilities[i] = std::exp(-Beta*4*J*i);
  }

  Initialize = true;
}

template <uint32_t D> void IsingModel<D>::UpdateSystem()
{
  uint32_t step = 0, print_steps = std::max(uint32_t(100),TotalSteps/1000+1);
  for (uint32_t i = 0; i < TotalSteps; i++) {
    CurrentStep = i;

    if (SaveInterval > 0 && i % SaveInterval == 0) {
      __Measure();
      __SaveSystem(step);
      step++;
    }

    if (i % print_steps == 0) {
      printf("Step: %d/%d\n", CurrentStep, TotalSteps);
    }

    for (uint32_t j = 0; j < System->TotalSize; j++) {
      __FlipSpin();
    }
  }
  printf("Step: %d/%d\n", CurrentStep+1, TotalSteps);
  __Measure();
  __SaveSystem(CurrentStep+1);
}

template <uint32_t D> void IsingModel<D>::LoadSystem(char const * filename)
{
  Json::Value Root;
  Json::Reader reader;
  std::ifstream savefile(filename, std::ifstream::in);
  savefile >> Root;
  savefile.close();

  assert(Root["Dimensions"].asUInt() == D);
  uint32_t * dim = new uint32_t[D], start_frame = Root["StartFrame"].asUInt();
  for (uint32_t i = 0; i < D; i++) { dim[i] = Root["Size"][i].asUInt(); }

  System = new SystemArray<bool, D>(dim);

  delete[] dim;

  StoredSystem = new bool*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    StoredSystem[i] = new bool[System->TotalSize];
  }

  for (uint32_t i = 0; i < D; i++) {
    Random[i].SetRange(0, System->Size[i] - 1);
  }

  for (uint32_t i = 0; i < System->TotalSize; i++) {
    System->SetLocal(Root["System"][start_frame][i].asBool(), i);
  }
}

template <uint32_t D> void IsingModel<D>::SaveSystem(char const * filename)
{
  Json::Value Root;
  Root["Dimensions"] = D;
  Root["Beta"] = Beta;
  Root["J"] = J;
  Root["TotalSteps"] = TotalSteps;
  Root["TotalSize"]  = System->TotalSize;
  Root["StartFrame"] = SavedSteps - 1;

  for (uint32_t i = 0; i < D; i++) {
    Root["Size"][i] = System->Size[i];
  }

  for (uint32_t i = 0; i < SavedSteps; i++) {
    for (uint32_t j = 0; j < System->TotalSize; j++) {
      Root["System"][i][j] = StoredSystem[i][j] ? 1 : 0;
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
