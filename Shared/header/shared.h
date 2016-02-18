#pragma once

//STL
#include <cstdint>
#include <cmath>
#include <cstring>
#include <ctime>
#include <chrono>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

// ThirdPary
#include "json.h"

/*
extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}
*/
typedef std::chrono::high_resolution_clock DefaultClock;
typedef std::chrono::time_point<DefaultClock> t_point;

/*
class LuaManager
{
private:
  Json::Value * Root;
  sel::State * lua;

public:
  LuaManager();
  ~LuaManager();

  void InitializeLua();
  template <typename T> void RegisterFunction(char const * name, T &f);
  void RunFile(char const * file);
};

template <typename T> void LuaManager::RegisterFunction(char const * name, T &f)
{
  if (lua == NULL) return;
  (*lua)[name] = f;
}
*/
