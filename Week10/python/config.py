#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json

def SaveInitFile(GM, t0, tf, dt, w_planet, r_surface, frict, angles, fix, filename):
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

  data["AnchorAngles"] = [cos(angles[0]), sin(angles[0]), sin(angles[1])*cos(angles[0]), sin(angles[1])*sin(angles[0]), cos(angles[1])*sin(angles[0]), cos(angles[1])*cos(angles[0])]

  try:
    f = open(filename, "w+")
    f.write(json.dumps(data))
    f.close()
  except Exception as e:
    print(e)

hours = 2**9
Nt = 2**15

GM = 5.165545706e12 #km^3 hour^-2 for Earth.
t0 = 0.0 # hours
dt = hours/Nt # hours
tf = hours-dt # hours
R_earth_equator = 6371 # km
W_earth = 0.2617993877991494 # hour^-1
theta = 0.0
theta_0 = 0.0

print("Hours: %3.1f\nTimesteps: %5d\ndt: %1.5f hours (%2.2f min)\n" % (hours, Nt, dt, dt*60.0))

SaveInitFile(GM, t0, tf, dt, W_earth, R_earth_equator, [1e2, 1.0/100.0, 0.0], [2*pi/360.0*theta, 2*pi/360.0*theta_0], True, "data/config/config-1.json")
