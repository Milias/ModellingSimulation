#!/usr/bin/env python
# -*- coding: utf8 -*-

import sys
import json
from numpy import *
import matplotlib.pyplot as plt

theo_c = lambda eta, r: (r<1.0)*(-(1+2*eta)**2 + 6*eta*(1+0.5*eta)**2*r-0.5*eta*(1+2*eta)**2*r**3)/(1.0-eta)**4

filenames = sys.argv[1:]
data = [json.loads(open(f,"r").read()) for f in filenames]

x = [linspace(0,d["GridPoints"]*d["DeltaR"],d["GridPoints"]) for d in data]
k = [linspace(0,pi/d["DeltaR"],d["GridPoints"]) for d in data]

c_r = [array(d["c_r"]) for d in data]
g_r = [array(d["g_r"]) for d in data]
s_k = [array(d["s_k"]) for d in data]

plt.title(r"$c(r)$")
for i, d in enumerate(data):
  print(d["Density"]*pi/6.0)
  y = theo_c(d["Density"]*pi/6.0, x[i])
  plt.plot(x[i], y, "--")#, label=r"Theory: $\eta = %1.2f$" % d["Density"]*pi/6.0)
  plt.plot(x[i], c_r[i], "-")#, label=r"Sim: $\eta = %1.2f$" % d["Density"]*pi/6.0)

plt.legend(loc=0)
plt.savefig("report/graphs/ozsolver-c.png")
plt.clf()

plt.title(r"$g(r)$")
for i, d in enumerate(data):
  plt.plot(x[i], g_r[i])#, label=r"$\eta = %1.2f$" % d["Density"]*pi/6.0)

plt.legend(loc=0)
plt.savefig("report/graphs/ozsolver-g.png")
plt.clf()

plt.title(r"$S(q)$")
for i, d in enumerate(data):
  plt.plot(k[i], s_k[i])# , label=r"$\eta = %1.2f$" % d["Density"]*pi/6.0)

plt.legend(loc=0)
plt.savefig("report/graphs/ozsolver-s.png")
plt.clf()
