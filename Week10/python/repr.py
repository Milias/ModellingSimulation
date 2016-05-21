#!/usr/bin/env python2
# -*- coding: utf8 -*-

import visual as vs
import json
import sys
from numpy import *

def cartesian(arrays, out=None):
    """
    Generate a cartesian product of input arrays.

    Parameters
    ----------
    arrays : list of array-like
        1-D arrays to form the cartesian product of.
    out : ndarray
        Array to place the cartesian product in.

    Returns
    -------
    out : ndarray
        2-D array of shape (M, len(arrays)) containing cartesian products
        formed of input arrays.

    Examples
    --------
    >>> cartesian(([1, 2, 3], [4, 5], [6, 7]))
    array([[1, 4, 6],
           [1, 4, 7],
           [1, 5, 6],
           [1, 5, 7],
           [2, 4, 6],
           [2, 4, 7],
           [2, 5, 6],
           [2, 5, 7],
           [3, 4, 6],
           [3, 4, 7],
           [3, 5, 6],
           [3, 5, 7]])

    """

    arrays = [asarray(x) for x in arrays]
    dtype = arrays[0].dtype

    n = prod([x.size for x in arrays])
    if out is None:
        out = zeros([n, len(arrays)], dtype=dtype)

    m = n / arrays[0].size
    out[:,0] = repeat(arrays[0], m)
    if arrays[1:]:
        cartesian(arrays[1:], out=out[0:m,1:])
        for j in xrange(1, arrays[0].size):
            out[j*m:(j+1)*m,1:] = out[0:m,1:]
    return out

def PlotIsingSystem2D(filename):
  try:
    f = open(filename,'r')
    data = json.loads(f.read())
    f.close()
  except Exception as e:
    return str(e)

  print("Beta: %f" % data["Beta"])

  scene = vs.display(title='3D representation',
    x=0, y=0, width=1440, height=1080,background=(0,0,0)
  )

  spin_up_color = array([0.1, 0.4, 0.8])
  spin_down_color = array([0.7, 0.7, 0.7])

  p_pos = cartesian([arange(0,data["Size"]) for i in range(2)])
  system_array = array(data["System"][0]).reshape(data["Size"], data["Size"])
  color_array = ((system_array == 1).reshape(data["Size"],data["Size"],1) * spin_up_color + (system_array == 0).reshape(data["Size"],data["Size"],1) * spin_down_color).reshape(data["TotalSize"], 3)

  p = vs.points(pos=p_pos-array([data["Size"]*0.5, data["Size"]*0.5]), size=15, shape="square", size_units="pixels", color=color_array)

  del p_pos
  del color_array
  del system_array

  nt = 0
  while True:
    vs.rate(60)
    system_array = array(data["System"][nt]).reshape(data["Size"], data["Size"])
    color_array = ((system_array == 1).reshape(data["Size"],data["Size"],1) * spin_up_color + (system_array == 0).reshape(data["Size"],data["Size"],1) * spin_down_color).reshape(data["TotalSize"], 3)

    p.color = color_array

    del system_array
    del color_array
    if nt + 1 >= data["SavedSteps"]:
      nt = 0
    else:
      nt += 1

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-2":
      print(PlotIsingSystem2D(sys.argv[2]))
    elif argv[1] == "-3":
      print("Nothing.")
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
