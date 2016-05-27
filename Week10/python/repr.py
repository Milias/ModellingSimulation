#!/usr/bin/env python2
# -*- coding: utf8 -*-

import visual as vs
import visual.graph as gr
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

  print(data["L0"])
  pos = []
  for i in range(data["SavedSteps"]):
    if not None in [elem for s1 in data["Position"][i] for elem in s1]:
      pos.append(data["Position"][i])
    else:
      break

  pos = array(pos)
  vel = array(data["Velocity"])
  scene = vs.display(title='3D representation', x=500, y=0, width=1920, height=1080,background=(0,0,0), center=pos[0][-1])
  string = vs.curve(pos=pos[0], radius=50)
  earth = vs.sphere(radius=R_earth_equator)
  asteroid = vs.sphere(pos=pos[0][-1],radius=1e3, color=vs.color.red)
  anchor = vs.sphere(pos=pos[0][0],radius=1e2, color=vs.color.green)

  label_avg_l0 = vs.label(pos=pos[0][-1], text="t: %3.1f" % (data["Time"][0],))

  r = sqrt(sum((pos[0,1:]-pos[0,:-1])**2,1))
  x = sqrt(sum((pos[0,1:])**2,1))
  #f1 = gr.gcurve(color=vs.color.cyan)
  #f1.plot(pos=array([x,r]).T)

  t = arange(data["T0"], data["Tf"], data["Dt"])
  f2 = gr.gcurve(color=vs.color.red)
  f2.plot(pos=(t[0], sqrt(sum(pos[0,-1])**2)))

  body = 1
  nt = 0
  while True:
    vs.rate(60)
    if scene.kb.keys: # event waiting to be processed?
        s = scene.kb.getkey() # get keyboard info
        if s == "d":
          if body == -1:
            body = 0
          elif body == 0:
            body = 1
          else:
            body = -1

    if body == 1:
      scene.center = (0,0,0)
    else:
      scene.center = pos[nt][body]
    string.pos=pos[nt]
    asteroid.pos=pos[nt][-1]
    anchor.pos=pos[nt][0]
    label_avg_l0.pos = asteroid.pos
    label_avg_l0.text = "t: %3.1f" % (data["Time"][nt],)
    #del r
    #del x
    #x = sqrt(sum((pos[nt,1:])**2,1))
    #r = sqrt(sum((pos[nt,1:]-pos[nt,:-1])**2,1))
    #f1.plot(pos=array([x,r]).T)
    f2.plot(pos=(t[nt], sqrt(sum(pos[nt,-1])**2)))
    if nt + 1 >= pos.shape[0]:
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
