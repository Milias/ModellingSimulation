#!/usr/bin/env python
# -*- coding: utf8 -*-
import importlib.util
spec = importlib.util.spec_from_file_location("graphs", "../Shared/python/graphs.py")
graphs = importlib.util.module_from_spec(spec)
spec.loader.exec_module(graphs)

from numpy import *
import matplotlib.pyplot as plt
import json
import sys

fig = plt.figure()

start = 0
kwargs = {"color":"red", "marker":"", "linestyle":"-"}
key = "ParticlesNumber"

filenames = sys.argv[1:]

"""
# Question 2.

ax = fig.add_subplot(1,1,1)

x = []
y = []
for n, f in enumerate(filenames):
  try:
    data = json.loads(open(f, "r").read())
    x.append(average(data["Density"][start:]))
    y.append(data["Mu"])
    del data
  except:
    continue

x = array(x)
y = array(y)

sorted_ind = argsort(x)

x = x[sorted_ind]
y = y[sorted_ind]

ax.plot(x,y, "r-", marker="o")
ax.set_xlabel(r"Density / $\rho\sigma^3$")
ax.set_ylabel(r"Chemical potential / $\mu$")
ax.set_title(r"$\mu$ vs Density - $\beta = 0.5$")
plt.savefig("report/graphs/mu_rho.pdf")
"""

"""
ax = fig.add_subplot(1,1,1)
for f in filenames:
  try:
    data = json.loads(open(f, "r").read())
    x = arange(0, data["TotalSteps"] + 1, data["SaveSystemInterval"])[start:]
    y = data[key][start:]
    ax.plot(x, y, "-", marker=".", label=r"$\mu: %1.1f, \rho_0: %1.1f$" % (data["Mu"], data["Density"][0]))
    del x, y, data
  except Exception as e:
    print(e)

ax.set_xlabel("Step")
ax.set_ylabel(key)
ax.legend(loc=0,numpoints=1)
"""

#"""
for n, f in enumerate(filenames):
  ax = fig.add_subplot(1,1,n+1)
  try:
    data = json.loads(open(f, "r").read())
    b = amax(data["ParticlesNumber"][start:])-amin(data["ParticlesNumber"][start:])
    hist, x = histogram(data[key][start:], bins=b, normed=False)
    x = x / data["Volume"]
    ax.bar(x[:-1], hist, width=(x[1]-x[0]))
    ax.set_title(r"$\mu: %1.3f, T^*: %1.3f$" % (data["Mu"], 1.0/data["Beta"]))
    del hist, x, data
  except Exception as e:
    ax.set_title(str(e))
#"""

"""
ax = fig.add_subplot(1,1,1)
titles = {"Pressure": "Pressure", "MuExcess": r"$\mu$", "Virial": "Virial", "Energy": "Energy"}
symbol = {"Pressure": "P", "MuExcess": r"\mu", "Virial": "V", "Energy": "E"}
key2 = "MuExcess"
x = []
y = [[],[],[]]
for i in range(8,25):
  if i == 15: continue
  x0, y0 = graphs.PlotQuantityVsQuantityAvg(["data/1/evol-fcc-%d-%d.json" % (i, j) for j in range(1,11)], "Density", key2, start, 1.0)
  x.append(x0)
  y[0].append(y0)
  y[1].append(graphs.PlotQuantityVsQuantityAvg(["data/2/evol-fcc-%d-%d.json" % (i, j) for j in range(1,11)], "Density", key2, start, 0.5)[1])
  y[2].append(graphs.PlotQuantityVsQuantityAvg(["data/0.5/evol-fcc-%d-%d.json" % (i, j) for j in range(1,11)], "Density", key2, start, 2.0)[1])

y = array(y)
x = array(x)

kwargs = [
  {"color":"r", "marker":"o", "linestyle":"-", "label":r"$k_B T / \epsilon = 1.0$"},
  {"color":"b", "marker":"o", "linestyle":"-", "label":r"$k_B T / \epsilon = 0.5$"},
  {"color":"m", "marker":"o", "linestyle":"-", "label":r"$k_B T / \epsilon = 2.0$"}]

for i in range(3):
  ax.plot(x,y[i],**kwargs[i])

plt.legend(loc=0,numpoints=1)
plt.xlabel(r"Density / $\rho \sigma^3$")
plt.ylabel(r"%s / $\langle %s \rangle$" % (titles[key2], symbol[key2]))
plt.title("%s vs Density" % titles[key2])
ax.legend(loc=0,numpoints=1)
plt.savefig("report/graphs/mu_rho_b.pdf")
"""
plt.show()
