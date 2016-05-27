#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json
import sys

GM = 5.165545706e12 #km^3 hour^-2 for Earth.
R_earth_equator = 6371.0 # km
W_earth = 0.2617993877991494 # hour^-1
R_go = (GM/W_earth**2)**(1/3.0)

N = 100
low_end_height = R_earth_equator
top_end_height = R_earth_equator + 90e3
L0 = (top_end_height-low_end_height)/N*0.9

young = 2e12*7.7e-8

print("Space end: %e km" % top_end_height)
print("Geosynchronus orbital speed: %e km/h" % (GM*W_earth)**(1/3.0))
print("Geosynchronus orbit: %e km" % R_go)

def StraightRotating(N, r0, rf, L0, w, SprK, RotK, rho, secc, angles, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  data = {}
  data["L0"] = L0
  data["ChainSize"] = N

  # x: equator to the east.
  # y: meridian to the north.
  # z: upwards.
  # left hand rule.

  x = linspace(0.0,1.0,N)
  rl = r0+(rf-r0)*x

  u_d = array([cos(angles[0]), sin(angles[1])*sin(angles[0]), cos(angles[1])*sin(angles[0])])

  r = (rl*reshape(u_d,(3,1))).T
  wl = w(rl)

  u_p = array([-u_d[1], u_d[0], u_d[2]])

  W = (wl*cross(r,u_p,axisc=1).T).T
  #W[-1,:] += u_d * 100
  data["Position"] = [list(r[i,:]) for i in range(N)]
  data["Velocity"] = [list(W[i,:]) for i in range(N)]
  data["Angle"] = [0.0]*N
  data["AngularVelocity"] = [0.0]*N

  data["SprK"] = list(SprK(x))
  data["RotK"] = list(RotK(x))
  data["Mass"] = list(rho(x)*L0)
  data["Mass"][-1] = 1e6
  data["Inertia"] = list(rho(r[:,2])*L0*(1/3.0*L0**2+secc(x)))

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

#W_earth*R_earth_equator*ones_like(r[:,2])

a = 0.4
b = 0.7
c = 0.4

f = lambda x: ((a-b)/(2*c-1)*(x**2-2*c*x)+a)/(a+c**2*(a-b)/(2*c-1))

theta = 0.0 * 2*pi/360.0
theta_0 = 0.0 * 2*pi/360.0

print(StraightRotating(N, low_end_height, top_end_height, L0, lambda r: -W_earth*ones_like(r), lambda x: young*L0*f(x), lambda r: 1.0*ones_like(r), lambda x: 12*f(x), lambda r: 7*1.3e-12*ones_like(r), [theta, theta_0], "data/init/sr-01.json"))
