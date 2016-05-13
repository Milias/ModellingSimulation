#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(beta, J, total_steps, save_interval, store_system, algorithm, auto_t, filename):
  data = {}
  data["Beta"] = beta
  data["J"] = J
  data["TotalSteps"] = total_steps
  data["SaveInterval"] = save_interval
  data["StoreSystem"] = store_system
  data["Metropolis"] = algorithm
  data["AutoT"] = auto_t

  try:
    f = open(filename, "w+")
    f.write(json.dumps(data))
    f.close()
    return "Successfully saved to %s." % filename
  except Exception as e:
    print(e)
    return e

nf = 0
for J in [1.0]:
  """
  for T in linspace(4.45, 4.65, 200):
    SaveInitFile(1/T, J, 20000, 5, False, True, (800, 1000), "data/config/config-%d.json" % nf)
    nf+=1
  """
  #"""
  for T in linspace(2.1, 2.5, 1):
    SaveInitFile(1/T, J, 1000, 1, False, False, (50, 1000), "data/config/config-%d.json" % nf)
    nf+=1
  #"""
