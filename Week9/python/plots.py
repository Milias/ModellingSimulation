#!/usr/bin/env python
# -*- coding: utf8 -*-

import json
import sys
from numpy import *
from scipy.optimize import curve_fit
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
  try:
    if data["Metropolis"]:
      plt.xlabel("Simulation steps per lattice point")
    else:
      plt.xlabel("Simulation steps")
  except:
    plt.xlabel("Simulation steps")

  plt.ylabel("Magnetization per lattice point")
  plt.plot(linspace(0, data["TotalSteps"], data["SavedSteps"]), M, "r-")
  plt.savefig(savefile)
  plt.show()
  return "Success."

def MagnetizationVsT(start, savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return str(e)

  start = int(start)

  M_l = {}
  for i in range(len(filenames)):
    E = array(data[i]["Magnetization"])[start:]
    if not data[i]["Size"] in M_l:
      M_l[data[i]["Size"]] = [[],[],[]]
    if not 1/data[i]["Beta"] in M_l[data[i]["Size"]][0]:
      M_l[data[i]["Size"]][0].append(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1].append(average(abs(array(data[i]["Magnetization"])[start:])/data[i]["TotalSize"]))
      M_l[data[i]["Size"]][2].append(1)
    else:
      M_l[data[i]["Size"]][1][M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])] += average(abs(array(data[i]["Magnetization"])[start:])/data[i]["TotalSize"])
      M_l[data[i]["Size"]][2][M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])] += 1
    del E

  plt.title("Magnetization vs Temperature")
  plt.xlabel(r"Temperature / $T^*$")
  plt.ylabel("Magnetization per lattice point")

  for i in M_l:
    x = array(M_l[i][0])
    y = array(M_l[i][1])
    N = array(M_l[i][2])
    y = y/N
    ind = argsort(x)
    x2 = x[ind]
    y2 = y[ind]
    M_l[i][0] = x2
    M_l[i][1] = y2
    plt.plot(M_l[i][0], M_l[i][1], "-", label="L = %d" % i, linewidth=1.2)

  plt.legend(loc=0,numpoints=1)
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

  M = array(data["Energy"])/data["TotalSize"]

  plt.title("Energy vs Step\n"+r"$T = %1.2f, J = %1.2f$" % (1/data["Beta"], data["J"]))
  try:
    if data["Metropolis"]:
      plt.xlabel("Simulation steps per lattice point")
    else:
      plt.xlabel("Simulation steps")
  except:
    plt.xlabel("Simulation steps")

  plt.ylabel("Energy per lattice point")
  plt.plot(linspace(0, data["TotalSteps"], data["SavedSteps"]), M, "r-")
  plt.savefig(savefile)
  plt.show()
  return "Success."

def EnergyVsT(start, savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return str(e)

  start = int(start)

  M_l = {}
  for i in range(len(filenames)):
    E = array(data[i]["Magnetization"])[start:]
    if not data[i]["Size"] in M_l:
      M_l[data[i]["Size"]] = [[],[],[]]
    if not 1/data[i]["Beta"] in M_l[data[i]["Size"]][0]:
      M_l[data[i]["Size"]][0].append(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1].append(average(array(data[i]["Energy"])[start:]/data[i]["TotalSize"]))
      M_l[data[i]["Size"]][2].append(1)
    else:
      M_l[data[i]["Size"]][1][M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])] += average(array(data[i]["Energy"])[start:]/data[i]["TotalSize"])
      M_l[data[i]["Size"]][2][M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])] += 1
    del E

  plt.title("Energy vs Temperature")
  plt.xlabel(r"Temperature / $T^*$")
  plt.ylabel("Energy per lattice point")

  for i in M_l:
    x = array(M_l[i][0])
    y = array(M_l[i][1])
    N = array(M_l[i][2])
    y = y/N
    ind = argsort(x)
    x2 = x[ind]
    y2 = y[ind]
    M_l[i][0] = x2
    M_l[i][1] = y2
    plt.plot(M_l[i][0], M_l[i][1], "-", label="L = %d" % i, linewidth=1.2)

  plt.legend(loc=0, numpoints=1)
  plt.savefig(savefile)
  plt.show()
  return "Success."

def HeatVsT(start, savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return str(e)

  start = int(start)

  M_l = {}
  for i in range(len(filenames)):
    E = array(data[i]["Magnetization"])[start:]
    if not data[i]["Size"] in M_l:
      M_l[data[i]["Size"]] = [[],[],[],[]]
    y = data[i]["Beta"]**2/data[i]["TotalSize"]*(average(E*E)-average(abs(E))**2)
    if not 1/data[i]["Beta"] in M_l[data[i]["Size"]][0]:
      M_l[data[i]["Size"]][0].append(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1].append(y)
      M_l[data[i]["Size"]][2].append(1)
      M_l[data[i]["Size"]][3].append(y**2)
    else:
      ind = M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1][ind] += y
      M_l[data[i]["Size"]][2][ind] += 1
      M_l[data[i]["Size"]][3][ind] += y**2
    del E

  plt.title("Specific heat vs Temperature")
  plt.xlabel(r"Temperature / $T^*$")
  plt.ylabel("Specific heat / $c_s(T)$")

  for i in M_l:
    x = array(M_l[i][0])
    y = array(M_l[i][1])
    N = array(M_l[i][2])
    y = y/N
    ind = argsort(x)
    x2 = x[ind]
    y2 = y[ind]
    M_l[i][0] = x2
    M_l[i][1] = y2
    plt.plot(M_l[i][0], M_l[i][1], "-", label="L = %d" % i, linewidth=1.2)

  plt.legend(loc=0, numpoints=1)
  plt.savefig(savefile)
  #plt.show()

  T_c = linspace(2.2,2.3,5)
  alpha = linspace(1.76,1.77,5)
  nu = linspace(0.99,1.01,5)

  """
  nf = 0
  for i in alpha:
    for j in nu:
      for k in T_c:
        print(nf)
        for m in M_l:
          t = (M_l[m][0]-k)/k
          #plt.plot(M_l[m][0], M_l[m][1], "-", label="L = %d" % m, linewidth=1.2)
          plt.plot(t*m**(1/j), M_l[m][1]*m**(-i/j), "-", label="L = %d" % m, linewidth=1.2)
          plt.xlim([-10,20])
        plt.title("Magnetic susceptibility vs Temperature\n"+r"$\gamma = %1.3f, \nu = %1.3f, T_c = %1.3f$" % (i, j, k))
        plt.legend(loc=0, numpoints=1)
        plt.savefig("report/graphs/fit-2/fit-%d.png" % nf)
        plt.clf()
        nf += 1
  """
  return "Success."

def SuscVsT(start, savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return str(e)

  start = int(start)

  M_l = {}
  for i in range(len(filenames)):
    E = array(data[i]["Magnetization"])[start:]
    if not data[i]["Size"] in M_l:
      M_l[data[i]["Size"]] = [[],[],[],[]]
    y = data[i]["Beta"]/data[i]["TotalSize"]*(average(E*E)-average(abs(E))**2)
    if not 1/data[i]["Beta"] in M_l[data[i]["Size"]][0]:
      M_l[data[i]["Size"]][0].append(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1].append(y)
      M_l[data[i]["Size"]][2].append(1)
      M_l[data[i]["Size"]][3].append(y**2)
    else:
      ind = M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1][ind] += y
      M_l[data[i]["Size"]][2][ind] += 1
      M_l[data[i]["Size"]][3][ind] += y**2
    del E

  del data
  plt.xlabel(r"Temperature / $T^*$")
  plt.ylabel(r"Magnetic susceptibility / $\chi(T)$")

  for i in M_l:
    x = array(M_l[i][0])
    y = array(M_l[i][1])
    N = array(M_l[i][2])
    z = array(M_l[i][3])
    y = y/N
    z = z/N
    ind = argsort(x)
    x2 = x[ind]
    y2 = y[ind]
    z2 = z[ind]
    M_l[i][0] = x2
    M_l[i][1] = y2
    M_l[i][3] = sqrt(z2-y2*y2)

  for m in M_l:
    plt.plot(M_l[m][0], M_l[m][1], "-", label="L = %d" % m, linewidth=1.2)
    plt.fill_between(M_l[m][0], M_l[m][1]-M_l[m][3], M_l[m][1]+M_l[m][3], interpolate=True)
  plt.title("Magnetic susceptibility vs Temperature\n")
  plt.legend(loc=0, numpoints=1)
  plt.savefig(savefile)
  plt.show()

  T_c = linspace(2.2,2.3,5)
  gamma = linspace(1.76,1.77,5)
  nu = linspace(0.99,1.01,5)

  """
  nf = 0
  for i in gamma:
    for j in nu:
      for k in T_c:
        print(nf)
        for m in M_l:
          t = (M_l[m][0]-k)/k
          #plt.plot(M_l[m][0], M_l[m][1], "-", label="L = %d" % m, linewidth=1.2)
          plt.plot(t*m**(1/j), M_l[m][1]*m**(-i/j), "-", label="L = %d" % m, linewidth=1.2)
          plt.xlim([-10,20])
        plt.title("Magnetic susceptibility vs Temperature\n"+r"$\gamma = %1.3f, \nu = %1.3f, T_c = %1.3f$" % (i, j, k))
        plt.legend(loc=0, numpoints=1)
        plt.savefig("report/graphs/fit-2/fit-%d.png" % nf)
        plt.clf()
        nf += 1
  """

  return "Success."

def AutovsN(savefile, filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  print("T = %f" % (1/data["Beta"]))
  auto_fun = abs(array(data["Autocorrelation"]))

  plt.plot(linspace(0,data["AutoT"][1]-data["AutoT"][0],len(auto_fun)), auto_fun, "r-", linewidth=1.2)
  plt.title("Autocorrelation function for $\chi(t)$")
  try:
    if data["Metropolis"]:
      plt.xlabel("Simulation steps per lattice point")
    else:
      plt.xlabel("Simulation steps")
  except:
    plt.xlabel("Simulation steps")
  plt.savefig(savefile)
  plt.show()
  return "Success."

def CorrvsT(end, savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return str(e)

  end = int(end)

  M_l = {}
  for i in range(len(filenames)):
    auto_fun = data[i]["Autocorrelation"]
    t = linspace(0,data[i]["AutoT"][1]-data[i]["AutoT"][0],len(auto_fun))
    y = trapz(auto_fun[:end], t[:end])

    if not data[i]["Size"] in M_l:
      M_l[data[i]["Size"]] = [[],[],[],[]]
    if not 1/data[i]["Beta"] in M_l[data[i]["Size"]][0]:
      M_l[data[i]["Size"]][0].append(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1].append(y)
      M_l[data[i]["Size"]][2].append(1)
      M_l[data[i]["Size"]][3].append(y**2)
    else:
      ind = M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1][ind] += y
      M_l[data[i]["Size"]][2][ind] += 1
      M_l[data[i]["Size"]][3][ind] += y**2
    del E

  M_l = {}
  for i in range(len(filenames)):
    auto_fun = data[i]["Autocorrelation"]
    t = linspace(0,data[i]["AutoT"][1]-data[i]["AutoT"][0],len(auto_fun))
    tau = trapz(auto_fun[:end], t[:end])

    if not data[i]["Size"] in M_l:
      M_l[data[i]["Size"]] = [[],[],[]]
    if not 1/data[i]["Beta"] in M_l[data[i]["Size"]][0]:
      M_l[data[i]["Size"]][0].append(1/data[i]["Beta"])
      M_l[data[i]["Size"]][1].append(tau)
      M_l[data[i]["Size"]][2].append(1)
    else:
      M_l[data[i]["Size"]][1][M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])] += tau
      M_l[data[i]["Size"]][2][M_l[data[i]["Size"]][0].index(1/data[i]["Beta"])] += 1
    del t
    del auto_fun

  del data
  plt.title("Correlation time vs Temperature")
  plt.xlabel(r"Temperature / $T^*$")
  plt.ylabel(r"Correlation time / $\tau(T)$")

  for i in M_l:
    x = array(M_l[i][0])
    y = array(M_l[i][1])
    N = array(M_l[i][2])
    y = y/N
    ind = argsort(x)
    x2 = x[ind]
    y2 = y[ind]
    M_l[i][0] = x2
    M_l[i][1] = y2
    plt.plot(M_l[i][0], M_l[i][1], "-", label="L = %d" % i, linewidth=1.2)

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
    elif argv[1] == "-XvsT":
      print(SuscVsT(sys.argv[2], sys.argv[3], sys.argv[4:]))
    elif argv[1] == "-CvsT":
      print(HeatVsT(sys.argv[2], sys.argv[3], sys.argv[4:]))
    elif argv[1] == "-AvsN":
      print(AutovsN(sys.argv[2], sys.argv[3]))
    elif argv[1] == "-TauvsT":
      print(CorrvsT(sys.argv[2], sys.argv[3], sys.argv[4:]))
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
