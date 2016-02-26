# -*- coding: utf8 -*-
from numpy import *
import json

def GenerateHexagonal(N, sph_size, scale, filename):
  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 2
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = [[scale, 0.0], [0.5*scale, sqrt(3)/2*scale]]
  data["Basis"] = basis
  basis = array(basis)

  data["Data"] = []
  for i in range(N):
    if i % 2 == 0:
      t = [0, i]
    else:
      t = [-1, i]
    for j in range(N):
      data["Data"].append([t[0] + j - i/2, i])
      data["SpheresNumber"] += 1

  v = array(data["Data"])
  v = basis[0]*v[:,0].reshape(data["SpheresNumber"],1) + basis[1]*v[:,1].reshape(data["SpheresNumber"],1)
  data["SystemSize"] = [
    [amin(v[:,0])-sph_size, amin(v[:,1])-sph_size],
    [amax(v[:,0]), amax(v[:,1])+sph_size]
  ]
  f = file(filename, "w+")
  f.write(json.dumps(data))
  f.close()

GenerateHexagonal(20, 1.0, 2.5, "data/lattice-hexa.json")
