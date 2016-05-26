#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(GM, t0, tf, dt, w_planet, r_surface, frict, fix, filename):
  data = {}
  data["GM"] = GM
  data["T0"] = t0
  data["Tf"] = tf
  data["Dt"] = dt
  data["WPlanet"] = w_planet
  data["RSurface"] = r_surface
  data["Fixed"] = fix
  data["FrictA"] = frict[0]
  data["FrictB"] = frict[1]
  data["FrictC"] = frict[2]

  try:
    f = open(filename, "w+")
    f.write(json.dumps(data))
    f.close()
  except Exception as e:
    print(e)

GM = 5.165545706e12 #km^3 hour^-2 for Earth.
t0 = 0.0 # hours
tf = 30.0 # hours
dt = 0.01 # hours
R_earth_equator = 6371 # km
W_earth = 0.2617993877991494 # hour^-1

SaveInitFile(GM, t0, tf, dt, W_earth, R_earth_equator, [1e2, 1.0/100.0, 0.0], True, "data/config/config-1.json")
