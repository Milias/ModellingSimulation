#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(GM, t0, tf, dt, filename):
  data = {}
  data["GM"] = GM
  data["T0"] = t0
  data["Tf"] = tf
  data["Dt"] = dt

  try:
    f = open(filename, "w+")
    f.write(json.dumps(data))
    f.close()
  except Exception as e:
    print(e)

GM = 5.165545706e12 #km^3 hour^-2 for Earth.
t0 = 0.0 # hours
tf = 50.0 # hours
dt = 0.1 # hours

SaveInitFile(GM, t0, tf, dt, "data/config/config-1.json")
