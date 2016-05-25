#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json
import sys

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

  data["SprK"] = list(SprK(r))
  data["RotK"] = list(RotK(r))
  data["Mass"] = list(rho(r)*L0)
  data["Inertia"] = list(rho(r)*L0*(1/3.0*L0**2+secc(r)))

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

R_earth_equator = 6371 # km
W_earth = 7.2921159e-5*3600 # hour^-1

print(StraightRotating(10000, 20000.0, 29999.0, 1.0, lambda r: array([2*W_earth*25000*ones_like(r[:,2]), zeros_like(r[:,0]), zeros_like(r[:,0])]).T, lambda r: 1.0*ones_like(r[:,0]), lambda r: 1e6*ones_like(r[:,0]), lambda r: 7*ones_like(r[:,0]), lambda r: 7*1.3e-12*ones_like(r[:,0]), "data/init/sr-01.json"))
