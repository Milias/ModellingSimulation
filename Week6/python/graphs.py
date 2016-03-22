#!/usr/bin/env python
# -*- coding: utf8 -*-
import importlib.util
spec = importlib.util.spec_from_file_location("graphs", "../Shared/python/graphs.py")
graphs = importlib.util.module_from_spec(spec)
spec.loader.exec_module(graphs)

from numpy import *
import json
import matplotlib.pyplot as plt

fig = plt.figure()

start = 1
kwargs = {"color":"red", "marker":".", "linestyle":"-"}

ax = fig.add_subplot(2,2,1)
avg = graphs.PlotQuantityVsStep("data/evol-fcc.json", "Pressure", ax, start, kwargs)
ax.set_title(r"Pressure - $\langle P \rangle = %.3f$" % avg)

ax = fig.add_subplot(2,2,2)
avg = graphs.PlotQuantityVsStep("data/evol-fcc.json", "MuExcess", ax, start, kwargs)
ax.set_title(r"MuExcess - $\langle \mu_{ex} \rangle = %.3f$" % avg)

ax = fig.add_subplot(2,2,3)
avg = graphs.PlotQuantityVsStep("data/evol-fcc.json", "Energy", ax, start, kwargs)
ax.set_title(r"Energy - $\langle E \rangle = %.3f$" % avg)

ax = fig.add_subplot(2,2,4)
avg = graphs.PlotQuantityVsStep("data/evol-fcc.json", "Virial", ax, start, kwargs)
ax.set_title(r"Virial - $\langle V \rangle = %.3f$" % avg)

plt.show()
