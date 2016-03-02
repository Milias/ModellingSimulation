# -*- coding: utf8 -*-
from numpy import *
import matplotlib.pyplot as plt
import sys
import json

if sys.version_info < (3, 0):
  import visual as vs

def PlotSpheres(f):
  data = json.loads(open(f, "r").read())
  plt.axis("equal")
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
  plt.axis([data["SystemSize"][0][0], data["SystemSize"][1][0],data["SystemSize"][0][1], data["SystemSize"][1][1]])

  for i in range(data["SpheresNumber"]):
    c = plt.Circle((data["Data"][0][i], data["Data"][1][i]), data["SphereSize"], color='k')
    plt.gca().add_artist(c)

def PlotSphereEvolution2(f):
  data = json.loads(open(f, "r").read())
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

def PlotSphereEvolution3(f):
  data = json.loads(open(f, "r").read())

  spheres = [vs.sphere(radius=data["SphereSize"],pos=(data["Data"][0][0][i], data["Data"][1][0][i], data["Data"][2][0][i])) for i in range(data["SpheresNumber"])]

for i in ["hexagonal"]:
  PlotSpheres("data/"+i+".json")
  plt.savefig("report/graphs/"+i+".eps")
  plt.close()

PlotSphereEvolution2("data/hexagonal-evolution.json")
