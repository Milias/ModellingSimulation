#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(beta, J, total_steps, save_interval, filename):
  data = {}
  data["Beta"] = beta
  data["J"] = J
  data["TotalSteps"] = total_steps
  data["SaveInterval"] = save_interval

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
  for T in [1.0]:
    SaveInitFile(1/T, J, 10, 1, "data/config/config-%d.json" % nf)
    nf+=1
