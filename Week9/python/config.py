#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(beta, J, total_steps, save_interval, store_system, filename):
  data = {}
  data["Beta"] = beta
  data["J"] = J
  data["TotalSteps"] = total_steps
  data["SaveInterval"] = save_interval
  data["StoreSystem"] = store_system

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
  for T in linspace(2.5, 2.7, 24):
    SaveInitFile(1/T, J, 1000, 1, False, "data/config/config-%d.json" % nf)
    nf+=1
