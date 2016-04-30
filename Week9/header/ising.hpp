#pragma once
#include "shared.h"

template <typename T, uint32_t D> struct SystemArray
{
  T * Array = nullptr;

  uint32_t
    Size = 0,
    TotalSize = 0;

  SystemArray(uint32_t L) {
    Size = L;
    TotalSize = std::pow(L,D);

    Array = new T[TotalSize];
  }

  ~SystemArray() {
    delete[] Array;
  }

  uint32_t GetIndex(uint32_t * loc) {
    uint32_t ind = loc[D-1];
    for (uint32_t i = 2; i <= D; i++) {
      ind *= Size;
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
    ClusterProb = 0.0,
    Beta = 0.0,
    J = 0.0;

  uint32_t
    TotalSteps = 0,
    SaveInterval = 0,
    SavedSteps = 0,
    CurrentStep = 0,
    * SpinStack = nullptr,
    * SpinStackPtr = nullptr;

  RandomGenerator Random;

  void __ProcessStack(uint32_t temp_loc);
  virtual void __Measure() {}
  void __SaveSystem(uint32_t step);
  void __PrintSystem();

public:
  IsingModel();
  ~IsingModel();

  virtual void InitializeFromFile(char const * filename);
  void UpdateSystem();
  void LoadSystem(char const * filename);
  void SaveSystem(char const * filename);
};





template <uint32_t D> IsingModel<D>::IsingModel() {}

template <uint32_t D> IsingModel<D>::~IsingModel()
{
  if (System) delete System;

  if (StoredSystem) {
    for (uint32_t i = 0; i < TotalSteps; i++) { delete[] StoredSystem[i]; }
    delete[] StoredSystem;
  }
}

template <uint32_t D> void IsingModel<D>::__ProcessStack(uint32_t temp_loc)
{
  // true : spin up (+), false: spin down (-)
  // █

  /*
    i+-1 % N_total
    i+-L % _N_total
    i+-L^2 % N_total
  */

  *SpinStackPtr++ = temp_loc;
  bool t_spin;
  uint32_t t_ind;

  while (SpinStackPtr != SpinStack) {
    temp_loc = *(--SpinStackPtr);
    for (uint32_t i = 0; i < D; i++) {
      t_ind = temp_loc + std::pow(System->Size, i) % System->Size;
      t_spin = System->GetLocal(t_ind);
      if (t_spin == temp_loc) {
        if (ClusterProb < Random.RandomProb()) {
          *SpinStackPtr++ = t_ind;
          System->SetLocal(t_ind, !t_spin);
        }
      }

      t_ind = std::pow(System->Size, i);
      t_ind = temp_loc > t_ind ? temp_loc - t_ind :
      if (temp_loc < t_ind) {

      }
      t_spin = System->GetLocal(t_ind);
      if (t_spin == temp_loc) {
        if (ClusterProb < Random.RandomProb()) {
          *SpinStackPtr++ = t_ind;
          System->SetLocal(t_ind, !t_spin);
        }
      }
    }
  }
}

template <uint32_t D> void IsingModel<D>::__SaveSystem(uint32_t step)
{
  for (uint32_t i = 0; i < System->TotalSize; i++) {
    StoredSystem[step][i] = System->GetLocal(i);
  }
}

template <uint32_t D> void IsingModel<D>::__PrintSystem()
{
  for (uint32_t i = 0; i < System->Size; i++) {
    for (uint32_t j = 0; j < System->Size; j++) {
      printf("\x1b[3%dm█\x1b[0m",System->GetLocal(i*System->Size+j));
    } printf("\n");
  } printf("\n");
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

  ClusterProb = 1 - std::exp(-2*Beta*J);

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
      __ProcessStack(Random.RandomInt());
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
  uint32_t start_frame = Root["StartFrame"].asUInt();

  System = new SystemArray<bool, D>(Root["Size"].asUInt());

  SpinStack = new uint32_t[System->TotalSize];
  SpinStackPtr = SpinStack;

  StoredSystem = new bool*[SavedSteps];
  for (uint32_t i = 0; i < SavedSteps; i++) {
    StoredSystem[i] = new bool[System->TotalSize];
  }

  Random.SetRange(0, System->TotalSize - 1);

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
  Root["SavedSteps"] = SavedSteps;
  Root["TotalSize"]  = System->TotalSize;
  Root["StartFrame"] = SavedSteps - 1;
  Root["Size"] = System->Size;

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
