# -*- coding: utf8 -*-
from numpy import *
import matplotlib.pyplot as plt
import sys
import json

if sys.version_info < (3, 0):
  import visual as vs

def PlotSpheres(f):
  data = json.loads(open(f, "r").read())
  plt.axis("equal")
  plt.plot([
    data["SystemSize"][0][0],
    data["SystemSize"][0][0],
    data["SystemSize"][1][0],
    data["SystemSize"][1][0],
    data["SystemSize"][0][0]],[

    data["SystemSize"][0][1],
    data["SystemSize"][1][1],
    data["SystemSize"][1][1],
    data["SystemSize"][0][1],
    data["SystemSize"][0][1]],
  'r-', linewidth=2.0)
  plt.axis([data["SystemSize"][0][0], data["SystemSize"][1][0],data["SystemSize"][0][1], data["SystemSize"][1][1]])

  for i in range(data["SpheresNumber"]):
    c = plt.Circle((data["Data"][0][i], data["Data"][1][i]), data["SphereSize"], color='k')
    plt.gca().add_artist(c)

def PlotSpheres3(f):
  data = json.loads(open(f, "r").read())

  scene = vs.display(title='3D representation',
    x=0, y=0, width=1920, height=1080,
    autocenter=True,background=(0,0,0))

  vs.box(pos=(
    (data["SystemSize"][1][0]+data["SystemSize"][0][0])*0.5,
    (data["SystemSize"][1][1]+data["SystemSize"][0][1])*0.5,
    (data["SystemSize"][1][2]+data["SystemSize"][0][2])*0.5
  ),
  length=data["SystemSize"][1][0]-data["SystemSize"][0][0],
  height=data["SystemSize"][1][1]-data["SystemSize"][0][1],
  width= data["SystemSize"][1][2]-data["SystemSize"][0][2],
  opacity=0.2,
  color=vs.color.red)

  spheres = [vs.sphere(radius=data["SphereSize"],pos=(data["Data"][0][i], data["Data"][1][i], data["Data"][2][i])) for i in range(data["SpheresNumber"])]

  vs.arrow(pos=data["SystemSize"][0], axis=(1,0,0), shaftwidth=0.1, color=vs.color.red)
  vs.arrow(pos=data["SystemSize"][0], axis=(0,1,0), shaftwidth=0.1, color=vs.color.green)
  vs.arrow(pos=data["SystemSize"][0], axis=(0,0,1), shaftwidth=0.1, color=vs.color.blue)

  while True:
    vs.rate(60)

def PlotSphereEvolution2(f):
  data = json.loads(open(f, "r").read())
  for j in range(data["SavedSteps"]):
    plt.clf()
    plt.axis("equal")
    plt.axis([data["SystemSize"][0][0], data["SystemSize"][1][0],data["SystemSize"][0][1], data["SystemSize"][1][1]])
    plt.plot([
      data["SystemSize"][0][0],
      data["SystemSize"][0][0],
      data["SystemSize"][1][0],
      data["SystemSize"][1][0],
      data["SystemSize"][0][0]],[

      data["SystemSize"][0][1],
      data["SystemSize"][1][1],
      data["SystemSize"][1][1],
      data["SystemSize"][0][1],
      data["SystemSize"][0][1]],
    'r-', linewidth=2.0)

    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i], data["Data"][1][j][i]), data["SphereSize"], color='k')
      plt.gca().add_artist(c)

    x0 = data["SystemSize"][1][0] - data["SystemSize"][0][0]
    y0 = data["SystemSize"][1][1] - data["SystemSize"][0][1]

    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i] + x0, data["Data"][1][j][i]), data["SphereSize"], color='k')
      plt.gca().add_artist(c)
    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i], data["Data"][1][j][i] + y0), data["SphereSize"], color='k')
      plt.gca().add_artist(c)
    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i] - x0, data["Data"][1][j][i]), data["SphereSize"], color='k')
      plt.gca().add_artist(c)
    for i in range(data["SpheresNumber"]):
      c = plt.Circle((data["Data"][0][j][i], data["Data"][1][j][i] - y0), data["SphereSize"], color='k')
      plt.gca().add_artist(c)


    plt.title("Frame: %04d" % j)
    plt.savefig("report/graphs/evolution-%04d.png" % j)

def PlotSphereEvolution3(f):
  data = json.loads(open(f, "r").read())

  center = (
    (data["SystemSize"][1][0]+data["SystemSize"][0][0])*0.5,
    (data["SystemSize"][1][1]+data["SystemSize"][0][1])*0.5,
    (data["SystemSize"][1][2]+data["SystemSize"][0][2])*0.5
  )

  scene = vs.display(title='3D representation',
    x=0, y=0, width=1920, height=1080,
    center=center,background=(0,0,0)
  )

  vs.box(pos=center,
  length=data["SystemSize"][1][0]-data["SystemSize"][0][0],
  height=data["SystemSize"][1][1]-data["SystemSize"][0][1],
  width= data["SystemSize"][1][2]-data["SystemSize"][0][2],
  opacity=0.2,
  color=vs.color.red)

  spheres = [vs.sphere(radius=data["SphereSize"],pos=(data["Data"][0][0][i], data["Data"][1][0][i], data["Data"][2][0][i])) for i in range(data["SpheresNumber"])]

  nt = 0
  while True:
    vs.rate(60)
    for i in range(data["SpheresNumber"]):
      spheres[i].pos = (data["Data"][0][nt][i], data["Data"][1][nt][i], data["Data"][2][nt][i])
    if nt + 1 >= data["SavedSteps"]:
      nt = 0
    else:
      nt += 1

def PlotRSquared(f):
  data = json.loads(open(f, "r").read())
  x = arange(data["SavedSteps"])
  y = array(data["Data"])

  rsq = array([average(sum((y[:,i,:]-y[:,0,:])*(y[:,i,:]-y[:,0,:]),axis=0)) for i in range(data["SavedSteps"])])

  plt.plot(x,rsq,'r-')

def PlotMelting(f):
  data = [json.loads(open(i, "r").read()) for i in f]

  x = []
  rsq = []
  for i in data:
    y = array(i["Data"])
    x.append(i["PackFraction"])
    rsq.append(average(sum((y[:,-1,:]-y[:,0,:])*(y[:,-1,:]-y[:,0,:]),axis=0)))

  plt.plot(x,rsq,'r-')

"""
for i in ["hexagonal", "rectangular"]:
  PlotSpheres("data/"+i+".json")
  plt.savefig("report/graphs/"+i+".eps")
  plt.close()

PlotRSquared("data/sc-evolution.json")
plt.savefig("report/graphs/sc-rsquared.eps")
plt.close()

PlotMelting(["data/hexagonal-evolution-%d.json" % i for i in range(1,100)])
plt.savefig("report/graphs/hexa-melting.eps")
plt.close()

PlotMelting(["data/fcc-evolution-%d.json" % i for i in range(1,100)])
plt.savefig("report/graphs/fcc-melting.eps")
plt.close()

if sys.version_info < (3, 0):
  PlotSpheres3("data/sc.json")
  #PlotSphereEvolution3("data/sc-evolution.json")
else:
  PlotSphereEvolution2("data/hexagonal-evolution.json")
"""
