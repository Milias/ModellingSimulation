# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(step_size, max_parts, beta, mu, r_cut, total_steps, save_interval, filename):
  data = {}
  data["MaxParticlesNumber"] = max_parts
  data["StepSize"] = step_size
  data["Beta"] = beta
  data["Mu"] = mu
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

nf = 0
for mu in linspace(-2.52661, 0, 1):
  for T in linspace(1.1, 0.95, 1):
    SaveInitFile(0.1, 400, 1.0/T, mu, 2.5, 500000, 100, "data/config/config-%d.json" % nf)
    nf+=1
