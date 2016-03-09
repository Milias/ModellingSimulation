#!/bin/python2
# -*- coding: utf8 -*-
from numpy import *
from scipy.special import *
import matplotlib.pyplot as plt
import sys
import json

if sys.version_info < (3, 0):
  import visual as vs

def SphereVolume(dim, R):
  return pi**(0.5*dim)/gamma(0.5*dim+1)*R**dim

def CubeVolume(xmin, xmax):
  v = 1.0
  for i in range(len(xmin)):
    v *= (xmax-xmin)[i]
  return v

def PlotSphereEvolution2(filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  for j in range(data["SavedSteps"]):
    plt.clf()
    plt.axis("equal")
    plt.axis([data["SystemSize"][0][0], data["SystemSize"][1][0],data["SystemSize"][0][1], data["SystemSize"][1][1]])
    plt.plot([
      data["SystemSize"][0][0],
      data["SystemSize"][0][0],
      data["SystemSize"][1][0],
      data["SystemSize"][1][0],
      data["SystemSize"][0][0]],[

      data["SystemSize"][0][1],
      data["SystemSize"][1][1],
      data["SystemSize"][1][1],
      data["SystemSize"][0][1],
      data["SystemSize"][0][1]],
    'r-', linewidth=2.0)

    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i], data["Data"][1][j][i]), data["SphereSize"], color='k')
      plt.gca().add_artist(c)

    x0 = data["SystemSize"][1][0] - data["SystemSize"][0][0]
    y0 = data["SystemSize"][1][1] - data["SystemSize"][0][1]

    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i] + x0, data["Data"][1][j][i]), data["SphereSize"], color='k')
      plt.gca().add_artist(c)
    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i], data["Data"][1][j][i] + y0), data["SphereSize"], color='k')
      plt.gca().add_artist(c)
    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i] - x0, data["Data"][1][j][i]), data["SphereSize"], color='k')
      plt.gca().add_artist(c)
    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i], data["Data"][1][j][i] - y0), data["SphereSize"], color='k')
      plt.gca().add_artist(c)

    plt.title("Frame: %04d" % j)
    plt.savefig("report/graphs/evolution-%04d.png" % j)

def PlotSphereEvolution3(filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  center = (
    (data["SystemSize"][0][1][0]+data["SystemSize"][0][0][0])*0.5,
    (data["SystemSize"][0][1][1]+data["SystemSize"][0][0][1])*0.5,
    (data["SystemSize"][0][1][2]+data["SystemSize"][0][0][2])*0.5
  )

  scene = vs.display(title='3D representation',
    x=0, y=0, width=1440, height=1080,
    center=center,background=(0,0,0)
  )

  boundary = vs.box(
    pos=center,
    length=data["SystemSize"][0][1][0]-data["SystemSize"][0][0][0],
    height=data["SystemSize"][0][1][1]-data["SystemSize"][0][0][1],
    width =data["SystemSize"][0][1][2]-data["SystemSize"][0][0][2],
    opacity=0.2,
    color=vs.color.red
  )

  spheres = [vs.sphere(radius=data["SphereSize"],pos=(data["Data"][0][i][0], data["Data"][0][i][1], data["Data"][0][i][2])) for i in range(data["SpheresNumber"])]

  nt = 1
  while True:
    vs.rate(60)
    boundary.pos = (
      (data["SystemSize"][nt][1][0]+data["SystemSize"][nt][0][0])*0.5,
      (data["SystemSize"][nt][1][1]+data["SystemSize"][nt][0][1])*0.5,
      (data["SystemSize"][nt][1][2]+data["SystemSize"][nt][0][2])*0.5
    )
    scene.center = boundary.pos
    boundary.length = data["SystemSize"][nt][1][0]-data["SystemSize"][nt][0][0]
    boundary.height = data["SystemSize"][nt][1][1]-data["SystemSize"][nt][0][1]
    boundary.width  = data["SystemSize"][nt][1][2]-data["SystemSize"][nt][0][2]
    for i in range(data["SpheresNumber"]):
      spheres[i].pos = (data["Data"][nt][i][0], data["Data"][nt][i][1], data["Data"][nt][i][2])
    if nt + 1 >= data["SavedSteps"]:
      nt = 0
    else:
      nt += 1

def PlotRSquared(filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  x = arange(data["SavedSteps"])
  y = array(data["Data"])

  rsq = array([average(sum((y[:,i,:]-y[:,0,:])*(y[:,i,:]-y[:,0,:]),axis=0)) for i in range(data["SavedSteps"])])

  plt.plot(x,rsq,'r-')

def PlotPackingFractionVsStep(f):
  data = json.loads(open(f, "r").read())

  y = []
  spheres_volume = data["SpheresNumber"]*SphereVolume(len(data["SystemSize"][0][0]), data["SphereSize"])
  for i in range(0, data["TotalSteps"], data["SaveSystemInterval"]):
    xmin = array(data["SystemSize"][i][0])
    xmax = array(data["SystemSize"][i][1])
    V = CubeVolume(xmin, xmax)
    y.append(V/spheres_volume)

  plt.plot(x,y,'r-')

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-plot2":
      print(PlotSphereEvolution2(argv[2]))
    elif argv[1] == "-plot3":
      print(PlotSphereEvolution3(argv[2]))
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
