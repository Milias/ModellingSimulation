#!/bin/python
# -*- coding: utf-8 -*-
from numpy import *
import matplotlib.pyplot as plt
import json
import sys

def PlotHistogram(filename, step):
  data = json.loads(open(filename, 'r').read())

<<<<<<< HEAD
  y = array(data["NormalizedDensity"])
  x = array(data["BinDistances"])
  y_avg = average(y[step:], axis=0)
  rho = average(array(data["BoxParticleDensity"]))
  plt.axis([0, data["BinDistances"][-1] + 0.5*data["BinWidth"], 0, max(y_avg)])
  plt.xlabel("r / Sphere diameter units")
  plt.ylabel("Probability density")
  plt.title("Normalized pair distances for\na FCC lattice with density " + r"$\rho$ = %1.2f" % rho)
  plt.bar(x+0.5*data["BinWidth"], y_avg, data["BinWidth"], edgecolor = "none")
  plt.savefig("report/graphs/fcc-%1.2f.pdf" % rho)
  plt.savefig("report/graphs/fcc-%1.2f.eps" % rho)
=======
  plt.bar(data["BinDistances"], data["NormalizedDensity"][step], data["BinWidth"])
  plt.show()
>>>>>>> 8ebc1ce7c21bb664fa948fa5f207337baaed4f64

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-hist" and len(argv) == 4:
      PlotHistogram(argv[2], int(argv[3]))
    else:
      print("Wrong argument.")
  else:
    print("No arguments.")

ParseInput(sys.argv)
