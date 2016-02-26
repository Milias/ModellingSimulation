# -*- coding: utf8 -*-
from numpy import *
import matplotlib.pyplot as plt
import json

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
      c = plt.Circle((data["Data"][0][j][i], data["Data"][1][j][i]), data["SphereSize"], color='k')
      plt.gca().add_artist(c)
    #plt.show()
    plt.savefig("report/graphs/evolution-%d.png" % j)

for i in ["hexagonal"]:
  PlotSpheres("data/"+i+".json")
  plt.savefig("report/graphs/"+i+".eps")
  plt.close()

PlotSphereEvolution2("data/hexagonal-evolution.json")
