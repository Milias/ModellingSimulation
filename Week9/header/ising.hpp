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

  bool
    ** StoredSystem = nullptr,
    StoreSystem = false,
    Metropolis = false;

  double
    ClusterProb = 0.0,
    Beta = 0.0,
    J = 0.0,
    E = 0.0,
    M = 0.0,
    * StoredM = nullptr,
    * StoredE = nullptr;

  uint32_t
    TotalSteps = 0,
    SaveInterval = 0,
    SavedSteps = 0,
    CurrentStep = 0,
    AutoTMax = 0,
    AutoTMin = 0,
    * SpinStack = nullptr,
    * SpinStackPtr = nullptr;

  RandomGenerator Random;

  void __MetropolisFlip();
  void __ProcessStack(uint32_t temp_loc);
  virtual void __Measure(uint32_t step);
  void __FlipSpin(uint32_t t_ind, bool spin);
  void __SaveSystem(uint32_t step);
  void __PrintSystem();
  double * __ComputeAutocorrelation();

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
  if (StoredE) delete[] StoredE;
  if (StoredM) delete[] StoredM;

  if (StoredSystem) {
    for (uint32_t i = 0; i < SavedSteps; i++) { delete[] StoredSystem[i]; }
    delete[] StoredSystem;
  }
}

template <uint32_t D> void IsingModel<D>::__FlipSpin(uint32_t t_ind, bool spin)
{
  bool t_spin = System->GetLocal(t_ind);
  if (t_spin == spin) {
    if (ClusterProb > Random.RandomProb()) {
      *SpinStackPtr++ = t_ind;
      System->SetLocal(!t_spin, t_ind);
    }
  }
}

template <uint32_t D> void IsingModel<D>::__MetropolisFlip()
{
  uint32_t spin_loc = Random.RandomInt(), L = 1;
  int32_t nn_spin = 0;
  for (uint32_t i = 0; i < D; i++) {
    nn_spin += System->GetLocal(spin_loc + L < System->TotalSize ? spin_loc + L : spin_loc + L - System->TotalSize) ? 1 : -1;
    nn_spin += System->GetLocal(spin_loc < L ? System->TotalSize - (L - spin_loc) : spin_loc - L) ? 1 : -1;
    L *= System->Size;
  }
  bool spin = System->GetLocal(spin_loc);
  double dE = 2*J*(spin ? 1 : -1)*nn_spin;
  if (dE > 0) {
    if (std::exp(-Beta*dE) > Random.RandomProb()) {
      E += dE;
      M += 2*(spin ? -1 : 1);
      System->SetLocal(!spin, spin_loc);
    }
  } else {
    E += dE;
    M += 2*(spin ? -1 : 1);
    System->SetLocal(!spin, spin_loc);
  }
}

template <uint32_t D> void IsingModel<D>::__ProcessStack(uint32_t temp_loc)
{
  // true : spin up (+1), false: spin down (-1)
  uint32_t L = 1;
  *SpinStackPtr++ = temp_loc;
  bool temp_loc_spin = System->GetLocal(temp_loc);
  System->SetLocal(!temp_loc_spin, temp_loc);

  while (SpinStackPtr != SpinStack) {
    temp_loc = *(--SpinStackPtr);
    if (System->GetLocal(temp_loc) != !temp_loc_spin) { continue; }

    L = 1;
    for (uint32_t i = 0; i < D; i++) {
      __FlipSpin(temp_loc + L < System->TotalSize ? temp_loc + L : temp_loc + L - System->TotalSize, temp_loc_spin);
      __FlipSpin(temp_loc < L ? System->TotalSize - (L - temp_loc) : temp_loc - L, temp_loc_spin);
      L *= System->Size;
    }
  }
}

template <uint32_t D> void IsingModel<D>::__SaveSystem(uint32_t step)
{
  if (step >= SavedSteps) {
    printf("Warning: step >= SavedSteps. (%d,%d)\n", step, SavedSteps);
    return;
  }

  for (uint32_t i = 0; i < System->TotalSize; i++) {
    StoredSystem[step][i] = System->GetLocal(i);
  }
}

template <uint32_t D> void IsingModel<D>::__Measure(uint32_t step)
{
  if (!Metropolis) {
    E = 0.0;
    M = 0.0;
    for (uint32_t i = 0; i < System->TotalSize; i++) {
      M += System->GetLocal(i) ? 1 : -1;
      for (uint32_t j = 0; j < D; j++) {
        E += System->GetLocal(i) && System->GetLocal(i + std::pow(System->Size, j) < System->TotalSize ? i + std::pow(System->Size, j) : i + std::pow(System->Size, j) - System->TotalSize) ? 1 : -1;
        E += System->GetLocal(i) && System->GetLocal(i < std::pow(System->Size, j) ? System->TotalSize - (std::pow(System->Size, j) - i) : i - std::pow(System->Size, j)) ? 1 : -1;
      }
    }
    E *= -J;
  }
  StoredE[step] = E;
  StoredM[step] = M;
}

template <uint32_t D> void IsingModel<D>::__PrintSystem()
{
  for (uint32_t i = 0; i < System->Size; i++) {
    for (uint32_t j = 0; j < System->Size; j++) {
      printf("\x1b[3%dm██\x1b[0m",System->GetLocal(j*System->Size+i) ? 1 : 2);
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
  StoreSystem = Root["StoreSystem"].asBool();
  Metropolis = Root["Metropolis"].asBool();
  AutoTMin = Root["AutoT"][0].asUInt();
  AutoTMax = Root["AutoT"][1].asUInt();
  SavedSteps = SaveInterval > 0 ? TotalSteps / SaveInterval + 1 : 0;

  StoredE = new double[SavedSteps];
  StoredM = new double[SavedSteps];

  ClusterProb = 1 - std::exp(-2*Beta*J);

  Initialize = true;
}

template <uint32_t D> void IsingModel<D>::UpdateSystem()
{
  uint32_t step = 0, print_steps = std::max(uint32_t(100),TotalSteps/1000+1);

  E = 0.0;
  M = 0.0;
  for (uint32_t i = 0; i < System->TotalSize; i++) {
    M += System->GetLocal(i) ? 1 : -1;
    for (uint32_t j = 0; j < D; j++) {
      E += System->GetLocal(i) && System->GetLocal(i + std::pow(System->Size, j) < System->TotalSize ? i + std::pow(System->Size, j) : i + std::pow(System->Size, j) - System->TotalSize) ? 1 : -1;
      E += System->GetLocal(i) && System->GetLocal(i < std::pow(System->Size, j) ? System->TotalSize - (std::pow(System->Size, j) - i) : i - std::pow(System->Size, j)) ? 1 : -1;
    }
  }
  E *= -J;

  for (uint32_t i = 0; i < TotalSteps; i++) {
    CurrentStep = i;

    if (SaveInterval > 0 && i % SaveInterval == 0) {
      __Measure(step);
      if (StoreSystem) { __SaveSystem(step); }
      step++;
    }

    if (i % 1000 == 0) {
      printf("Step: %d/%d\n", CurrentStep, TotalSteps);
    }

    if (Metropolis) {
      for (uint32_t i = 0; i < System->TotalSize; i++) {
        __MetropolisFlip();
      }
    } else {
      __ProcessStack(Random.RandomInt());
    }
  }
  printf("Step: %d/%d\n", CurrentStep+1, TotalSteps);
  __Measure(step);
  if (StoreSystem) { __SaveSystem(SavedSteps-1); }
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

template <uint32_t D> double * IsingModel<D>::__ComputeAutocorrelation()
{
  uint32_t n_points = AutoTMax - AutoTMin;
  double * auto_fun = new double[n_points];
  double t1 = 0.0, t2 = 0.0;
  for (uint32_t i = 0; i < n_points; i++) {
    t1 = 0.0; t2 = 0.0;
    for (uint32_t j = 0; j < n_points - i; j++) {
      t1 += StoredM[j+AutoTMin]*StoredM[j+i+AutoTMin];
    }
    auto_fun[i] = t1/(n_points - i);
    t1 = 0.0;
    for (uint32_t j = 0; j < n_points - i; j++) {
      t1 += StoredM[j+AutoTMin];
      t2 += StoredM[j+i+AutoTMin];
    }
    auto_fun[i] -= t1/(n_points - i)*t2/(n_points - i);
    if (i > 0) auto_fun[i] /= auto_fun[0];
  }
  auto_fun[0] = 1.0;
  return auto_fun;
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

  Root["AutoT"][0] = AutoTMin;
  Root["AutoT"][1] = AutoTMax;

  double * autocorrelation = __ComputeAutocorrelation();

  for (uint32_t i = 0; i < SavedSteps; i++) {
    Root["Energy"][i] = StoredE[i];
    Root["Magnetization"][i] = StoredM[i];
  }

  for (uint32_t i = 0; i < AutoTMax - AutoTMin; i++) {
    Root["Autocorrelation"][i] = autocorrelation[i];
  }

  delete[] autocorrelation;

  if (StoreSystem) {
    for (uint32_t i = 0; i < SavedSteps; i++) {
      for (uint32_t j = 0; j < System->TotalSize; j++) {
        Root["System"][i][j] = StoredSystem[i][j] ? 1 : 0;
      }
    }
  }

  Json::FastWriter writer;
  std::ofstream savefile(filename);
  savefile << writer.write(Root);
  savefile.close();
}
