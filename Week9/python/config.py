#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(beta, J, total_steps, save_interval, store_system, algorithm, filename):
  data = {}
  data["Beta"] = beta
  data["J"] = J
  data["TotalSteps"] = total_steps
  data["SaveInterval"] = save_interval
  data["StoreSystem"] = store_system
  data["Metropolis"] = algorithm

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
  for T in linspace(1.0, 4.0, 100):
    SaveInitFile(1/T, J, 40000, 5, False, True, "data/config/config-%d.json" % nf)
    nf+=1
  """
  for T in linspace(2.1, 2.5, 200):
    SaveInitFile(1/T, J, 40000, 5, False, True, "data/config/config-%d.json" % nf)
    nf+=1
