# -*- coding: utf8 -*-
from numpy import *
import matplotlib.pyplot as plt
import json
import matplotlib.cm as cm
from matplotlib.colors import LogNorm
from scipy.optimize import curve_fit

def PlotRSquared(f):
  data = json.loads(open(f, 'r').read())
  plt.plot(arange(1,data["StepNumber"]+1), data["Data"], '-', label=data["Legend"])

  box = ""
  if (data["BoxSize"] > 0.0):
    box = " - Box size: %1.f" % data["BoxSize"]

  plt.title(r"$\langle R^2(N) \rangle$ vs. $N$ in %dD%s" % (data["Dimensions"],box)+"\n"+"Average over $10^{%d}$ walks" % (log10(data["WalksNumber"]),))
  plt.xlabel("N / step")
  plt.ylabel(r"$\langle R^2(N) \rangle$ / AU")
  plt.legend(loc=0)

def PlotRandomWalk2(f):
  data = json.loads(open(f, 'r').read())
  plt.plot(data["Data"][0], data["Data"][1], '-')

  box = ""
  if (data["BoxSize"] > 0.0):
    box = " - Box size: %1.f" % data["BoxSize"]

  plt.title("Random walk in %dD - $10^{%d}$ steps\n%s%s" % (data["Dimensions"], log10(data["StepNumber"]), data["Legend"], box))
  plt.xlabel("x / AU")
  plt.ylabel("y / AU")
  plt.axis('equal')

def PlotColorMap2(f, b = 200):
  data = json.loads(open(f, 'r').read())
  H, xedges, yedges = histogram2d(data["Data"][1], data["Data"][0], bins=b)
  a = average(H)
  d = sqrt(average((H-a)**2))

  plt.title("Random walk in %dD - $10^{%d}$ steps\nAverage density: %2.1f$\pm$%2.1f (%d by %d grid)" % (data["Dimensions"], log10(data["StepNumber"]), a, d, b, b))
  plt.xlabel("Deviation from average")
  plt.ylabel("Probability density")
  n, bins = histogram((H-a).ravel(), bins=60, density=True)
  plt.bar(bins[:-1], n, (amax(H-a)-amin(H-a))/60.0, color='b')
  fit = lambda x, a, b: a*exp(-b*x**2)
  popt, pcov = curve_fit(fit, bins[:-1], n, p0=(amax(n), 0.001))

  print(popt)

  x = linspace(amin(H-a), amax(H-a), 100)
  l = plt.plot(x, fit(x, *popt), 'r--', linewidth=1)

  plt.savefig("report/graphs/%s-hist.eps" % f[5:-5])
  plt.close()

  plt.axis([xedges[0], xedges[-1], yedges[0], yedges[-1]])
  plt.title("Random walk in %dD - $10^{%d}$ steps\nAverage density: %2.1f$\pm$%2.1f (%d by %d grid)" % (data["Dimensions"], log10(data["StepNumber"]), a, d, b, b))
  plt.xlabel("x / AU")
  plt.ylabel("y / AU")
  plt.imshow(H, interpolation='bilinear', origin='low', extent=[xedges[0], xedges[-1], yedges[0], yedges[-1]], cmap=cm.hot)
  plt.colorbar()

def PlotColorMap2multi(f, N, b = 200):
  mdata = [ json.loads(open(i, 'r').read()) for i in f ]
  x = array([d["Data"][0] for d in mdata]).ravel()
  y = array([d["Data"][1] for d in mdata]).ravel()
  H, xedges, yedges = histogram2d(y, x, bins=b)
  a = average(H)
  d = sqrt(average((H-a)**2))
  plt.title("Random walk in %dD - %d steps, %d walks\nAverage density: %2.1f$\pm$%2.1f (%d by %d grid)" % (mdata[0]["Dimensions"], mdata[0]["StepNumber"], N, a, d, b, b))
  plt.xlabel("x / AU")
  plt.ylabel("y / AU")
  plt.imshow(H, interpolation='bilinear', origin='low', extent=[xedges[0], xedges[-1], yedges[0], yedges[-1]], cmap=cm.hot)
  plt.axis([xedges[0], xedges[-1], yedges[0], yedges[-1]])
  plt.colorbar()

"""
for i in ["data/rsquared2.json", "data/prsquared2.json"]: PlotRSquared(i)
plt.savefig("report/graphs/rsquared2.eps")
plt.close()

for i in ["data/rsquared3.json", "data/prsquared3.json"]: PlotRSquared(i)
plt.savefig("report/graphs/rsquared3.eps")
plt.close()

for i in ["data/rsquared11.json", "data/prsquared11.json"]: PlotRSquared(i)
plt.savefig("report/graphs/rsquared11.eps")
plt.close()

for i in ["data/rsquared1.json", "data/prsquared1.json"]: PlotRSquared(i)
plt.savefig("report/graphs/rsquared1.eps")
plt.close()

for i in ["data/randomwalk2.json"]: PlotRandomWalk2(i)
plt.savefig("report/graphs/randomwalk2.eps")
plt.close()

for i in ["data/randomwalk3.json"]: PlotRandomWalk2(i)
plt.savefig("report/graphs/randomwalk3.eps")
plt.close()

for i in ["prandomwalk2"]:
  PlotColorMap2("data/%s.json" % i, 200)
  plt.savefig("report/graphs/%smap.eps" % i)
  plt.close()
"""

N = 50000
s = "prandomwalk2"
f = ["data/bc/%s-%d.json" % (s, i) for i in range(1,N+1)]
PlotColorMap2multi(f, N, 200)
plt.savefig("report/graphs/%smap-b.eps" % s)
plt.close()
