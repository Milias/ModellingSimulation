#!/usr/bin/env python
# -*- coding: utf8 -*-

import json
import sys
from numpy import *
import matplotlib.pyplot as plt

def MagnetizationVsStep(savefile, filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  M = abs(array(data["Magnetization"]))/data["TotalSize"]

  plt.title("Magnetization vs Step\n"+r"$T = %1.2f, J = %1.2f$" % (1/data["Beta"], data["J"]))
  plt.xlabel("Steps per lattice point")
  plt.ylabel("Magnetization per lattice point")
  plt.plot(linspace(0, data["TotalSteps"], data["SavedSteps"]), M, "r-")
  plt.savefig(savefile)
  plt.show()
  return "Success."

def MagnetizationVsT(start, savefile, filenames):
  try:
    f = [open(filename,'r') for filename in filenames]
    data = [json.loads(i.read()) for i in f]
    for i in f: i.close()
  except Exception as e:
    return str(e)

  start = int(start)

  M = zeros(len(filenames))
  T = zeros(len(filenames))
  J = []
  for i in range(len(filenames)):
    M[i] = average(abs(array(data[i]["Magnetization"])[start:])/data[i]["TotalSize"])
    T[i] = 1/data[i]["Beta"]
    J.append(data[i]["J"])

  plt.title("Magnetization vs Temperature")
  plt.xlabel(r"Temperature / $T^*$")
  plt.ylabel("Magnetization per lattice point")
  plt.plot(T,M,"ro")
  plt.savefig(savefile)
  plt.show()
  return "Success."

def EnergyVsStep(savefile, filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  M = abs(array(data["Energy"]))/data["TotalSize"]

  plt.title("Energy vs Step\n"+r"$T = %1.2f, J = %1.2f$" % (1/data["Beta"], data["J"]))
  plt.xlabel("Steps per lattice point")
  plt.ylabel("Energy per lattice point")
  plt.plot(linspace(0, data["TotalSteps"], data["SavedSteps"]), M, "r-")
  plt.savefig(savefile)
  plt.show()
  return "Success."

def EnergyVsT(start, savefile, filenames):
  try:
    f = [open(filename,'r') for filename in filenames]
    data = [json.loads(i.read()) for i in f]
    for i in f: i.close()
  except Exception as e:
    return str(e)

  start = int(start)

  M = zeros(len(filenames))
  T = zeros(len(filenames))
  J = []
  for i in range(len(filenames)):
    M[i] = average(abs(array(data[i]["Energy"])[start:])/data[i]["TotalSize"])
    T[i] = 1/data[i]["Beta"]
    J.append(data[i]["J"])

  plt.title("Energy vs Temperature")
  plt.xlabel(r"Temperature / $T^*$")
  plt.ylabel("Energy per lattice point")
  plt.plot(T,M,"ro",label="J = %1.2f" % J[0])
  plt.legend(loc=0, numpoints=1)
  plt.savefig(savefile)
  plt.show()
  return "Success."

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-MvsN":
      print(MagnetizationVsStep(sys.argv[2], sys.argv[3]))
    elif argv[1] == "-MvsT":
      print(MagnetizationVsT(sys.argv[2], sys.argv[3], sys.argv[4:]))
    elif argv[1] == "-EvsN":
      print(EnergyVsStep(sys.argv[2], sys.argv[3]))
    elif argv[1] == "-EvsT":
      print(EnergyVsT(sys.argv[2], sys.argv[3], sys.argv[4:]))
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
