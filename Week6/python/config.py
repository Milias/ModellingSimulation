# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(part_moves, step_size, beta, mu_tests, r_cut, total_steps, save_interval, filename):
  data = {}
  data["ParticleMoves"] = part_moves
  data["StepSize"] = step_size
  data["Beta"] = beta
  data["MuExcessTests"] = mu_tests
  data["RCut"] = r_cut
  data["TotalSteps"] = total_steps
  data["SaveSystemInterval"] = save_interval

  try:
    f = open(filename, "w+")
    f.write(json.dumps(data))
    f.close()
    return "Successfully saved to %s." % filename
  except Exception as e:
    print(e)
    return e

for i, T in enumerate([1.0, 2.0]):
  SaveInitFile(0, 0.1, 1.0/T, 1000, 2.5, 100000, 100, "data/config/config-%d.json" % i)
