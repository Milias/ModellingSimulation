#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json
import sys

GM = 5.165545706e12 #km^3 hour^-2 for Earth.
R_earth_equator = 6371.0 # km
W_earth = 0.2617993877991494 # hour^-1
R_go = (GM/W_earth**2)**(1/3.0)

N = 30
low_end_height = R_earth_equator
top_end_height = R_earth_equator + 100e3
L0 = (top_end_height-low_end_height)/N

print("Space end: %e km" % top_end_height)
print("Geosynchronus orbital speed: %e km/h" % (GM*W_earth)**(1/3.0))
print("Geosynchronus orbit: %e km" % R_go)

def StraightRotating(N, r0, rf, L0, w, SprK, RotK, rho, secc, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  r = zeros((N,3))
  r[:,2] = linspace(r0,rf,N)

  data = {}
  data["L0"] = L0
  data["ChainSize"] = N

  # x: equator to the east.
  # y: meridian to the north.
  # z: upwards.
  # left hand rule.

  W = w(r)
  data["Position"] = [list(r[i,:]) for i in range(N)]
  data["Velocity"] = [list(W[i,:]) for i in range(N)]
  data["Angle"] = [0.0]*N
  data["AngularVelocity"] = [0.0]*N

  x = (r[:,2]-low_end_height)/(top_end_height-low_end_height)
  data["SprK"] = list(SprK(x))
  data["RotK"] = list(RotK(x))
  data["Mass"] = list(rho(x)*L0)
  data["Mass"][-1] = 1e6
  data["Inertia"] = list(rho(r[:,2])*L0*(1/3.0*L0**2+secc(x)))

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

#W_earth*R_earth_equator*ones_like(r[:,2])

print(StraightRotating(N, low_end_height, top_end_height, L0, lambda r: array([-W_earth*r[:,2], zeros_like(r[:,0]), zeros_like(r[:,2])]).T, lambda x: 1e8*(-(x+0.1)**2+x+1), lambda r: 1.0*ones_like(r), lambda x: 10*(-(x+0.1)**2+x+1), lambda r: 7*1.3e-12*ones_like(r), "data/init/sr-01.json"))
