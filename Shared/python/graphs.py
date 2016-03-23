#!/usr/bin/env python
# -*- coding: utf8 -*-

from numpy import *
import matplotlib.pyplot as plt
import sys
import json

def PlotQuantityVsStep(filename, key, ax, start, kwargs):
  try:
    f = open(filename, "r")
  except Exception as e:
    print(e)
    return 0.0

  data = json.loads(f.read())
  f.close()

  avg = average(data[key][start:])
  x = arange(0, data["TotalSteps"] + 1, data["SaveSystemInterval"])
  ax.plot(x[start:], data[key][start:], **kwargs)
  return avg

def PlotQuantityVsQuantity(filename, key1, key2, start):
  try:
    f = open(filename, "r")
  except Exception as e:
    print(e)
    return 0.0

  data = json.loads(f.read())
  f.close()

  avg = average(data[key2][start:])
  return (data[key1],avg)
