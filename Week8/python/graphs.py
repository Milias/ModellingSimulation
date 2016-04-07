#!/usr/bin/env python
# -*- coding: utf8 -*-

import sys
import json
from numpy import *
import matplotlib.pyplot as plt

start = 1

filenames = sys.argv[1:]
data = [json.loads(open(f,"r").read()) for f in filenames]

x = [linspace(0,d["GridPoints"]*d["DeltaR"],d["GridPoints"]) for d in data]
k = [linspace(0,pi/d["DeltaR"],d["GridPoints"]) for d in data]

c_r = [array(d["c_r"]) for d in data]
g_r = [array(d["g_r"]) for d in data]
s_k = [array(d["s_k"]) for d in data]
c_t = [array(d["c_t"]) for d in data]
g_t = [array(d["g_t"]) for d in data]
s_t = [array(d["s_t"]) for d in data]

c = ["r", "g", "b", "k", "m", "y", "r", "g", "b"]

plt.title(r"$c(r)$ - Direct correlation function")
for i, d in enumerate(data):
  plt.plot(x[i][start:], c_t[i][start:], c[i]+"-")
  plt.plot(x[i][start::8], c_r[i][start::8], c[i]+".", label=r"$\eta = %1.2f$" % (d["Density"]*pi/6.0))

plt.xlabel(r"Distance / $r/\sigma$")
plt.ylabel(r"Direct correlation function / $c(r)$")
plt.axis([0,1,-25,0])
plt.legend(loc=0, numpoints=1)
plt.savefig("report/graphs/ozsolver-c.pdf")
#plt.show()
plt.clf()

plt.title(r"$g(r)$ - Pair correlation function")
for i, d in enumerate(data):
  plt.plot(x[i][start:], g_t[i][start:], c[i]+"-")
  plt.plot(x[i][start::8], g_r[i][start::8], c[i]+".", label=r"$\eta = %1.2f$" % (d["Density"]*pi/6.0))

plt.xlabel(r"Distance / $r/\sigma$")
plt.ylabel(r"Pair correlation function / $g(r)$")
plt.axis([0,3,0,3.5])
plt.legend(loc=0, numpoints=1)
plt.savefig("report/graphs/ozsolver-g.pdf")
#plt.show()
plt.clf()

plt.title(r"$S(q)$ - Structure factor")
for i, d in enumerate(data):
  plt.plot(k[i][start:], s_t[i][start:], c[i]+"-")
  plt.plot(k[i][start:], s_k[i][start:], c[i]+".", label=r"$\eta = %1.2f$" % (d["Density"]*pi/6.0))

plt.xlabel(r"Momentum / $q\sigma$")
plt.ylabel(r"Structure factor / $S(q)$")
plt.axis([0,25,0,2.5])
plt.legend(loc=0, numpoints=1)
plt.savefig("report/graphs/ozsolver-s.pdf")
#plt.show()
plt.clf()
