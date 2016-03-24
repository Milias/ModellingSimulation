#!/usr/bin/env python
# -*- coding: utf8 -*-
import importlib.util
spec = importlib.util.spec_from_file_location("graphs", "../Shared/python/graphs.py")
graphs = importlib.util.module_from_spec(spec)
spec.loader.exec_module(graphs)

from numpy import *
import matplotlib.pyplot as plt


fig = plt.figure()

start = 20
kwargs = {"color":"red", "marker":"", "linestyle":"-"}

"""
filename ="data/1/evol-fcc-9.json"
ax = fig.add_subplot(2,2,1)
avg = graphs.PlotQuantityVsStep(filename, "Pressure", ax, start, kwargs)
ax.set_title(r"Pressure - $\langle P \rangle = %.3f$" % avg)

ax = fig.add_subplot(2,2,2)
avg = graphs.PlotQuantityVsStep(filename, "MuExcess", ax, start, kwargs)
ax.set_title(r"MuExcess - $\langle \mu_{ex} \rangle = %.3f$" % avg)

ax = fig.add_subplot(2,2,3)
avg = graphs.PlotQuantityVsStep(filename, "Energy", ax, start, kwargs)
ax.set_title(r"Energy - $\langle E \rangle = %.3f$" % avg)

ax = fig.add_subplot(2,2,4)
avg = graphs.PlotQuantityVsStep(filename, "Virial", ax, start, kwargs)
ax.set_title(r"Virial - $\langle V \rangle = %.3f$" % avg)
"""
#"""
ax = fig.add_subplot(1,1,1)
titles = {"Pressure": "Pressure", "MuExcess": r"$\mu$", "Virial": "Virial", "Energy": "Energy"}
symbol = {"Pressure": "P", "MuExcess": r"\mu", "Virial": "V", "Energy": "E"}
key2 = "MuExcess"
x = []
y = [[],[],[]]
for i in range(5,25):
  if i == 15: continue
  x0, y0 = graphs.PlotQuantityVsQuantityAvg(["data/1/evol-fcc-%d-%d.json" % (i, j) for j in range(1,11)], "Density", key2, start, 1.0)
  x.append(x0)
  y[0].append(y0)
  y[1].append(graphs.PlotQuantityVsQuantityAvg(["data/2/evol-fcc-%d-%d.json" % (i, j) for j in range(1,11)], "Density", key2, start, 2.0)[1])
  y[2].append(graphs.PlotQuantityVsQuantityAvg(["data/0.5/evol-fcc-%d-%d.json" % (i, j) for j in range(1,11)], "Density", key2, start, 0.5)[1])

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
plt.savefig("report/graphs/mu_rho_b.pdf")
#"""
plt.show()
