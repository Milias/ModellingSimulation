# -*- coding: utf8 -*-
from numpy import *
import json

def RecursiveSC(level, N, count, points):
  if level:
    for i in range(N[level]):
      RecursiveSC(level - 1, N, count, points)
      count[level] += 1
    count[level] = 0

  else:
    for i in range(N[0]):
      points.append(count[:])
      count[0] += 1
    count[0] = 0

def GenerateSC(dim, part_radius, N, scale, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  data = {}
  data["Dimensions"] = dim
  data["SavedSteps"] = 1
  data["ParticlesRadius"] = part_radius

  basis = scale * eye(dim)

  lattice_points = []
  RecursiveSC(dim - 1, N, [0] * dim, lattice_points)
  data["ParticlesNumber"] = len(lattice_points)
  lattice_points = array(lattice_points)

  data["Particles"] = [[]]
  for p in lattice_points:
    data["Particles"][0].append(list(sum(basis * p,axis=1)))

  particles = array(data["Particles"][0])
  data["SystemSize"] = [list(amin(particles, axis=0) - 0.001), list(amax(particles, axis=0) + 0.001)]

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

def GenerateFCC(N, part_radius, scale, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  data = {}
  data["Dimensions"] = 3
  data["SavedSteps"] = 1
  data["ParticlesRadius"] = part_radius

  basis = scale * array([[0.5,0.5,0.0],[0.5,0.0,0.5],[0.0,0.5,0.5]])

  lattice_points = []
  for i in range(N[0]):
    lattice_points.append((i, i, -i))
    lattice_points.append((i+1,i,-i))
    lattice_points.append((i,i+1,-i))
    lattice_points.append((i,i,-i+1))
    for j in range(N[1]):
      lattice_points.append((i+j, i-j, -i+j))
      lattice_points.append((i+j+1,i-j,j-i))
      lattice_points.append((i+j,i-j+1,j-i))
      lattice_points.append((i+j,i-j,j-i+1))
      for k in range(N[2]):
        lattice_points.append((i+j-k, i-j+k, -i+j+k))
        lattice_points.append((i+j-k+1,i-j+k,j-i+k))
        lattice_points.append((i+j-k,i-j+k+1,j-i+k))
        lattice_points.append((i+j-k,i-j+k,j-i+k+1))

  lattice_points = list(set(lattice_points))
  data["ParticlesNumber"] = len(lattice_points)
  lattice_points = array(lattice_points)

  data["Particles"] = [[]]
  for p in lattice_points:
    data["Particles"][0].append(list(sum(basis * p, axis=1)))

  particles = array(data["Particles"][0])
  data["SystemSize"] = [list(amin(particles, axis=0) - (part_radius + 0.001)), list(amax(particles, axis=0) + (part_radius + 0.001))]

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

for i, x in enumerate(linspace(2.0,5.0,10)):
  GenerateFCC([3]*3, 1.0, x, "data/density/fcc-%d.json" % (i+15))
