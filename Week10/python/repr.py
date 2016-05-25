#!/usr/bin/env python2
# -*- coding: utf8 -*-

import visual as vs
import json
import sys
from numpy import *

R_earth_equator = 6371 # km

def PlotElevator(filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  scene = vs.display(title='3D representation',
    x=0, y=0, width=1440, height=900,background=(0,0,0)
  )

  pos = array(data["Position"])
  string = vs.curve(pos=pos[0], radius=10)
  earth = vs.sphere(radius=R_earth_equator)

  nt = 1
  while True:
    vs.rate(60)
    string.pos=pos[nt]
    if nt + 1 >= data["SavedSteps"]:
      nt = 0
    else:
      nt += 1

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "--elevator":
      print(PlotElevator(sys.argv[2]))
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
