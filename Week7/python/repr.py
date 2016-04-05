#!/usr/bin/env python2
# -*- coding: utf8 -*-

import sys
import json

if sys.version_info < (3, 0):
  import visual as vs
else:
  print("Warning: Python-Visual not found.")

def PlotSphereEvolutionNVT(filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  center = (
    (data["SystemSize"][1][0]+data["SystemSize"][0][0])*0.5,
    (data["SystemSize"][1][1]+data["SystemSize"][0][1])*0.5,
    (data["SystemSize"][1][2]+data["SystemSize"][0][2])*0.5
  )

  scene = vs.display(title='3D representation',
    x=0, y=0, width=1440, height=1080,
    center=center,background=(0,0,0)
  )

  boundary = vs.box(
    pos=center,
    length=data["SystemSize"][1][0]-data["SystemSize"][0][0],
    height=data["SystemSize"][1][1]-data["SystemSize"][0][1],
    width =data["SystemSize"][1][2]-data["SystemSize"][0][2],
    opacity=0.2,
    color=vs.color.red
  )

  spheres = [vs.sphere(radius=data["ParticlesRadius"],pos=(data["Particles"][0][i][0], data["Particles"][0][i][1], data["Particles"][0][i][2])) for i in range(data["ParticlesNumber"])]

  nt = 1
  while True:
    vs.rate(60)
    for i in range(data["ParticlesNumber"]):
      spheres[i].pos = (data["Particles"][nt][i][0], data["Particles"][nt][i][1], data["Particles"][nt][i][2])
    if nt + 1 >= data["SavedSteps"]:
      nt = 0
    else:
      nt += 1

def PlotSphereEvolutionMuVT(filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  center = (
    (data["SystemSize"][1][0]+data["SystemSize"][0][0])*0.5,
    (data["SystemSize"][1][1]+data["SystemSize"][0][1])*0.5,
    (data["SystemSize"][1][2]+data["SystemSize"][0][2])*0.5
  )

  scene = vs.display(title='3D representation',
    x=0, y=0, width=1440, height=1080,
    center=center,background=(0,0,0)
  )

  boundary = vs.box(
    pos=center,
    length=data["SystemSize"][1][0]-data["SystemSize"][0][0],
    height=data["SystemSize"][1][1]-data["SystemSize"][0][1],
    width =data["SystemSize"][1][2]-data["SystemSize"][0][2],
    opacity=0.2,
    color=vs.color.red
  )

  spheres = [vs.sphere(radius=data["ParticlesRadius"]) for i in range(data["MaxParticlesNumber"])]
  for i in spheres:
    i.visible = False

  nt = 0
  while True:
    vs.rate(20)
    for i in range(data["MaxParticlesNumber"]):
      spheres[i].visible = False

    for i in range(data["ParticlesNumber"][nt]):
      spheres[i].pos = (data["Particles"][nt][i][0], data["Particles"][nt][i][1], data["Particles"][nt][i][2])
      spheres[i].visible = True

    if nt + 1 >= data["SavedSteps"]:
      nt = 0
    else:
      nt += 1

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-muvt":
      print(PlotSphereEvolutionMuVT(argv[2]))
    elif argv[1] == "-nvt":
      print(PlotSphereEvolutionNVT(argv[2]))
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
