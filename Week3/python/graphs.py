# -*- coding: utf8 -*-
from numpy import *
import matplotlib.pyplot as plt
import json

def PlotSpheres(f):
  data = json.loads(open(f, "r").read())
  x0 = -data["SphereSize"]
  xf = amax(data["Data"][0]) + data["SphereSize"]
  yf = amax(data["Data"][1]) + data["SphereSize"]
  plt.plot([x0, xf], [x0, yf], '.', ms=0.0)
  plt.axis("equal")
  for i in range(data["SpheresNumber"]):
    c = plt.Circle((data["Data"][0][i], data["Data"][1][i]), data["SphereSize"], color='k')
    plt.gca().add_artist(c)

for i in ["rect2a", "hexa", "rect2b", "hexb"]:
  PlotSpheres("data/"+i+".json")
  plt.savefig("report/graphs/"+i+".eps")
  plt.close()
