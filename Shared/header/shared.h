#pragma once

//STL
#include <cstdint>
#include <cstring>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <fstream>

// ThirdPary
#include "json.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#include "deprecated.h"
#include "math.hpp"

/*
  Convenience structure to manage step size
  adaptation to maintain roughly 50% hit rate.
*/
struct StepSizeAdapter
{
  uint32_t hits = 0, count = 0;
  double rate = 0.0, delta = 0.0;
  double min = 0.0, max = 0.0;

  StepSizeAdapter(double delta0, double min_bound, double max_bound) : delta(delta0), min(min_bound), max(max_bound) {}

  double Update(bool sum) {
    if (sum) { hits++; }
    count++;
    rate = 1.0 * hits / (count + 1);
    delta *= 1.0 + (rate - 0.5)/std::pow(count,0.5);
    delta = delta < min ? min : (delta > max ? max : delta);
    return delta;
  }
};
