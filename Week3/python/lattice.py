# -*- coding: utf8 -*-
from numpy import *
import json

def GenerateHexagonal(N, sph_size, scale, filename):
  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 2
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = [[scale, 0.0], [0.5*scale, 0.5*sqrt(3)*scale]]
  data["Basis"] = basis
  basis = array(basis).reshape(2,2,1)

  data["Data"] = []
  for i in range(N[0]):
    data["Data"].append([i,0])
    data["Data"].append([i,1])
    data["SpheresNumber"] += 2
    for j in range(1,N[1]):
      data["Data"].append([i-j,2*j])
      data["Data"].append([i-j,2*j+1])
      data["SpheresNumber"] += 2

  v = array(data["Data"])
  v = basis[0]*v[:,0].reshape(1,data["SpheresNumber"]) + basis[1]*v[:,1].reshape(1,data["SpheresNumber"])
  data["SystemSize"] = [
    [amin(v[:,i])-sph_size for i in range(data["Dimensions"])],
    [amax(v[:,i])+sph_size for i in range(data["Dimensions"])]
  ]
  f = open(filename, "w+")
  f.write(json.dumps(data))
  f.close()

def GenerateSC(N, sph_size, scale, filename):
  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 3
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = array([[scale,0.0,0.0],[0.0,scale,0.0],[0.0,0.0,scale]]).reshape(3,3,1)

  data["Basis"] = basis.reshape(3,3).tolist()

  data["Data"] = []
  for i in range(N[0]):
    for j in range(N[1]):
      for k in range(N[2]):
        data["Data"].append([i, j, k])
        data["SpheresNumber"] += 1

  v = array(data["Data"])
  v = basis[0]*v[:,0].reshape(1,data["SpheresNumber"]) + basis[1]*v[:,1].reshape(1,data["SpheresNumber"]) + basis[2]*v[:,2].reshape(1,data["SpheresNumber"])
  data["SystemSize"] = [
    [amin(v[:,i])-scale-sph_size for i in range(data["Dimensions"])],
    [amax(v[:,i])+sph_size+scale for i in range(data["Dimensions"])]
  ]

  f = open(filename, "w+")
  f.write(json.dumps(data))
  f.close()

def GenerateFCC(N, sph_size, scale, filename):
  data = {}
  data["FileType"] = "LatticeGenerator"
  data["Dimensions"] = 3
  data["SpheresNumber"] = 0
  data["SphereSize"] = sph_size

  basis = array([[0.5*scale,0.5*scale,0.0],[0.5*scale,0.0,0.5*scale],[0.0,0.5*scale,0.5*scale]]).reshape(3,3,1)

  data["Basis"] = basis.reshape(3,3).tolist()

  data["Data"] = []
  for i in range(N[0]):
    data["Data"].append([i, i, -i])
    data["Data"].append([i+1,i,-i])
    data["Data"].append([i,i+1,-i])
    data["Data"].append([i,i,-i+1])
    data["SpheresNumber"] += 4
    for j in range(N[1]):
      data["Data"].append([i+j, i-j, j-i])
      data["Data"].append([i+j+1,i-j,j-i])
      data["Data"].append([i+j,i-j+1,j-i])
      data["Data"].append([i+j,i-j,j-i+1])
      data["SpheresNumber"] += 4
      for k in range(N[2]):
        data["Data"].append([i+j-k, i-j+k, j-i+k])
        data["Data"].append([i+j-k+1,i-j+k,j-i+k])
        data["Data"].append([i+j-k,i-j+k+1,j-i+k])
        data["Data"].append([i+j-k,i-j+k,j-i+k+1])
        data["SpheresNumber"] += 4

  v = array(data["Data"])
  v = basis[0]*v[:,0].reshape(1,data["SpheresNumber"]) + basis[1]*v[:,1].reshape(1,data["SpheresNumber"]) + basis[2]*v[:,2].reshape(1,data["SpheresNumber"])
  data["SystemSize"] = [
    [amin(v[:,i])-scale-sph_size for i in range(data["Dimensions"])],
    [amax(v[:,i])+sph_size+scale for i in range(data["Dimensions"])]
  ]

  f = open(filename, "w+")
  f.write(json.dumps(data))
  f.close()

x = linspace(2.0,4.0,100)

GenerateHexagonal([10, 6], 1.0, 2.0, "data/lattice-hexa.json")
for i in range(100): GenerateHexagonal([10, 6], 1.0, x[i], "data/lattice-hexa-%d.json" % (i+1,))
GenerateSC([12]*3, 1.0, 2.1 , "data/lattice-sc.json")
GenerateFCC([6]*3, 1.0, 3.5 , "data/lattice-fcc.json")
for i in range(100): GenerateHexagonal([6]*3, 1.0, x[i], "data/lattice-fcc-%d.json" % (i+1,))
