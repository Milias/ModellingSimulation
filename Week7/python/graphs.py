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

start = 100

filenames = sys.argv[1:]

"""
# Question 2.

ax = fig.add_subplot(1,1,1)

x = [[], [], []]
y = [[], [], []]
for n, f in enumerate(filenames):
  try:
    data = json.loads(open(f, "r").read())
    if abs(1/data["Beta"]-2.0)<1e-5:
      y[0].append(data["Mu"])
      x[0].append(average(data["Density"][start:]))
    elif abs(1/data["Beta"]-1.0)<1e-5:
      y[1].append(data["Mu"])
      x[1].append(average(data["Density"][start:]))
    elif abs(1/data["Beta"]-0.5)<1e-5:
      y[2].append(data["Mu"])
      x[2].append(average(data["Density"][start:]))
    del data
    print("%d, %d, %d" % (len(y[0]), len(y[1]), len(y[2])))
  except Exception as e:
    print(e)

xmax = 0.7

try:
  data = json.loads(open("data/state.json", "r").read())

  y2 = array(data["y"])
  x2 = array(data["x"])

  kwargs2 = [
    {"color":"r", "marker":"^", "linestyle":"--"},
    {"color":"m", "marker":"^", "linestyle":"--"},
    {"color":"b", "marker":"^", "linestyle":"--"}]

  for i in [0, 2]:
    ax.plot(x2[x2<xmax], y2[i][x2<xmax],**kwargs2[i])
except Exception as e:
  print(e)

kwargs = [
  {"color":"b", "marker":"o", "linestyle":"-", "label":r"$k_B T / \epsilon = 2.0$"},
  {"color":"r", "marker":"o", "linestyle":"-", "label":r"$k_B T / \epsilon = 1.0$"},
  {"color":"m", "marker":"o", "linestyle":"-", "label":r"$k_B T / \epsilon = 0.5$"}]

x = [array(x[i]) for i in range(len(x))]
y = [array(y[i]) for i in range(len(y))]

sorted_ind = [argsort(x[i]) for i in range(len(x))]

x = array([x[i][sorted_ind[i]] for i in range(len(x))])
y = array([y[i][sorted_ind[i]] for i in range(len(y))])

for i in range(len(x)):
  if len(x[i]) == 0: continue
  ax.plot(x[i][x[i]<xmax], y[i][x[i]<xmax], **kwargs[i])

ax.set_xlabel(r"Density / $\rho\sigma^3$")
ax.set_ylabel(r"Chemical potential / $\mu$")
ax.set_title(r"$\mu$ vs Density")
plt.axis([0, xmax, -7, 7])
plt.legend(loc=0, numpoints=1)
plt.savefig("report/graphs/mu_rho_low.png")
plt.savefig("report/graphs/mu_rho_low.pdf")
plt.clf()
"""

"""
key = "ParticlesNumber"
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

"""
# Question 3
key = "ParticlesNumber"
for n, f in enumerate(filenames):
  ax = fig.add_subplot(1,1,1)
  print("\r%d" % (n+1), end="")
  try:
    data = json.loads(open(f, "r").read())
    b = amax(data["ParticlesNumber"][start:])-amin(data["ParticlesNumber"][start:])
    hist, x = histogram(data[key][start:], bins=b, normed=False)
    x = x / data["Volume"]
    plt.axis([0,amax(x),0,amax(hist)])
    ax.bar(x[:-1], hist, width=(x[1]-x[0]))
    ax.set_title(r"$\mu: %1.3f, T^*: %1.3f$" % (data["Mu"], 1.0/data["Beta"]))
    #plt.savefig("report/graphs/hists/hist_%1.5f_%1.5f.png" % (data["Mu"], 1.0/data["Beta"]))
    plt.savefig("report/graphs/hists-close/hist_%1.5f_%1.5f.pdf" % (data["Mu"], 1.0/data["Beta"]))
    plt.clf()
    del hist, x, data
  except Exception as e:
    ax.set_title(str(e))

print("")
"""

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

#"""
ax = fig.add_subplot(1,1,1)
x = linspace(0.85, 0.95, 12)
lp = array([0.11, 0.12, 0.13, 0.15, 0.16, 0.17, 0.18, 0.17, 0.20, 0.19, 0.20, 0.22])
rp = array([0.59, 0.59, 0.55, 0.56, 0.53, 0.52, 0.47, 0.45, 0.43, 0.32, 0.27, 0.22])

ax.plot(x,lp,**{"color":"r", "marker":"o", "linestyle":"-", "label":r"Left peak"})
ax.plot(x,rp,**{"color":"b", "marker":"o", "linestyle":"-", "label":r"Right peak"})
plt.legend(loc=0,numpoints=1)
plt.xlabel(r"Temperature / $T^*$")
plt.ylabel(r"Density / $\rho \sigma^3$")
plt.title("$T^*$ vs Density")
ax.legend(loc=0,numpoints=1)
plt.savefig("report/graphs/d_peak_rho.pdf")
#"""
