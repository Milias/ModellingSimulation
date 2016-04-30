#!/usr/bin/env python
# -*- coding: utf8 -*-
from numpy import *
import json
import sys

def IsingUniformState(value, size, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  size = [int(i) for i in size]
  total_size = prod(size)
  system = value*ones(total_size)

  data = {}
  data["Size"] = size[0]
  data["Dimensions"] = len(size)
  data["SavedSteps"] = 1
  data["System"] = [list(system)]
  data["StartFrame"] = 0

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

def IsingRandomState(value, size, filename):
  try:
    f = open(filename, "w+")
  except Exception as e:
    return str(e)

  size = [int(i) for i in size]
  total_size = prod(size)
  system = random.randint(0, value+1, total_size)

  data = {}
  data["Size"] = size[0]
  data["Dimensions"] = len(size)
  data["SavedSteps"] = 1
  data["System"] = [[int(i) for i in system]]
  data["StartFrame"] = 0

  f.write(json.dumps(data))
  f.close()
  return "Saved to %s successfully." %  filename

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-u":
      print(IsingUniformState(int(sys.argv[2]), sys.argv[3:-1], sys.argv[-1]))
    elif argv[1] == "-r":
      print(IsingRandomState(int(sys.argv[2]), sys.argv[3:-1], sys.argv[-1]))
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
