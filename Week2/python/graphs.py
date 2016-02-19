# -*- coding: utf8 -*-
from numpy import *
import matplotlib.pyplot as plt
import json

def PlotRSquared(files, plotfile):
  f = [ open(i, 'r') for i in files ]
  f_str = [ i.read() for i in f ]
  data = [ json.loads(i) for i in f_str ]

  plt.plot(arange(data[0]["StepNumber"]), data[0]["Data"], 'r-', label="Without periodic b. c.")
  plt.plot(arange(data[1]["StepNumber"]), data[1]["Data"], 'b-', label="With periodic b. c.")

  plt.legend(loc=0)
  plt.savefig(plotfile)
  plt.close()

def PlotRandomWalk(files, plotfile):
  f = [ open(i, 'r') for i in files ]
  f_str = [ i.read() for i in f ]
  data = [ json.loads(i) for i in f_str ]

  plt.plot(data[0]["Data"][0], data[0]["Data"][1], 'r-', label="Without periodic b. c.")
  plt.plot(data[1]["Data"][0], data[1]["Data"][1], 'b-', label="With periodic b. c.")

  plt.axis('equal')
  plt.legend(loc=0)
  plt.savefig(plotfile)
  plt.close()

PlotRSquared(["data/rsquared2.json", "data/prsquared2.json"], "report/graphs/rsquared2.eps")
PlotRSquared(["data/rsquared3.json", "data/prsquared3.json"], "report/graphs/rsquared3.eps")
PlotRandomWalk(["data/randomwalk2.json", "data/prandomwalk2.json"], "report/graphs/randomwalk2.eps")
