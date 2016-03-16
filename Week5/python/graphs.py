#!/bin/python
# -*- coding: utf-8 -*-
from numpy import *
import matplotlib.pyplot as plt
import json
import sys

def PlotHistogram(filename, step):
  data = json.loads(open(filename, 'r').read())

  plt.bar(data["BinDistances"], data["NormalizedDensity"][step], data["BinWidth"])
  plt.show()

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-hist" and len(argv) == 4:
      PlotHistogram(argv[2], int(argv[3]))
    else:
      print("Wrong argument.")
  else:
    print("No arguments.")

ParseInput(sys.argv)
