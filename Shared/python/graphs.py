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

def PlotQuantityVsQuantityAvg(filenames, key1, key2, start, beta):
  try:
    f = [open(filename, "r") for filename in filenames]
  except Exception as e:
    print(e)
    return 0.0

  data = [json.loads(i.read()) for i in f]
  for i in f: i.close()

  avg = [average(i[key2][start:]) for i in data]
  avg = [i for i in avg if float('Inf') != i]
  avg = average(avg)
  if key2 == "MuExcess":
    avg += log(data[0]["Density"])/beta
  return (data[0][key1],avg)
