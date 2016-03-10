# -*- coding: utf8 -*-
from numpy import *
import json

def GenerateRectangular(N, sph_size, scale, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 2
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = [[scale, 0.0], [0.0, scale]]
  data["Basis"] = basis

  data["Data"] = []
  for i in range(N[0]):
    for j in range(N[1]):
      data["Data"].append([i,j])
      data["SpheresNumber"] += 1

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

def GenerateHexagonal(N, sph_size, scale, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 2
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = [[scale, 0.0], [0.5*scale, 0.5*sqrt(3)*scale]]
  data["Basis"] = basis

  data["Data"] = []
  for i in range(N[0]):
    data["Data"].append([i,0])
    data["Data"].append([i,1])
    data["SpheresNumber"] += 2
    for j in range(1,N[1]):
      data["Data"].append([i-j,2*j])
      data["Data"].append([i-j,2*j+1])
      data["SpheresNumber"] += 2

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

def GenerateSC(N, sph_size, scale, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 3
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = [[scale,0.0,0.0],[0.0,scale,0.0],[0.0,0.0,scale]]
  data["Basis"] = basis

  data["Data"] = []
  for i in range(N[0]):
    for j in range(N[1]):
      for k in range(N[2]):
        data["Data"].append([i, j, k])
        data["SpheresNumber"] += 1

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

def GenerateFCC(N, sph_size, scale, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 3
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = [[0.5*scale,0.5*scale,0.0],[0.5*scale,0.0,0.5*scale],[0.0,0.5*scale,0.5*scale]]
  data["Basis"] = basis

  data["Data"] = []
  for i in range(N[0]):
    data["Data"].append((i, i, -i))
    data["Data"].append((i+1,i,-i))
    data["Data"].append((i,i+1,-i))
    data["Data"].append((i,i,-i+1))
    for j in range(N[1]):
      data["Data"].append((i+j, i-j, j-i))
      data["Data"].append((i+j+1,i-j,j-i))
      data["Data"].append((i+j,i-j+1,j-i))
      data["Data"].append((i+j,i-j,j-i+1))
      for k in range(N[2]):
        data["Data"].append((i+j-k, i-j+k, j-i+k))
        data["Data"].append((i+j-k+1,i-j+k,j-i+k))
        data["Data"].append((i+j-k,i-j+k+1,j-i+k))
        data["Data"].append((i+j-k,i-j+k,j-i+k+1))

  data["Data"] = list(set(data["Data"]))
  data["SpheresNumber"] = len(data["Data"])
  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename
