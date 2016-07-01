#!/usr/bin/env python
# -*- coding: utf8 -*-

import json
import sys
from numpy import *
from numpy.fft import *
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def CWDistance(savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return "Error loading: %s" % str(e)

  pos = []
  for i in range(len(data)):
    pos.append(array(data[i]["Position"]))

  y = [sqrt(sum(p[:,-1]**2,1)) for p in pos]

  plt.title("Counter-Weight distance from the center of Earth")
  plt.xlabel("Time / hour")
  plt.ylabel("Distance / $10^3$ km")
  for i in range(len(y)): plt.plot(data[i]["Time"], y[i]/1e3)
  plt.show()

  return "Success."

def AverageDisplacement(savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return "Error loading: %s" % str(e)

  pos = []
  for i in range(len(data)):
    pos.append(array(data[i]["Position"]))

  p1 = [p[:,-1]-p[:,0] for p in pos]
  R_0 = [sum(p1[i]**2,1) for i in range(len(pos))]
  p2 = [(p[:,0].reshape(p[:,0].shape+(1,))-p.transpose(0,2,1)).transpose(1,0,2) for p in pos]
  dp = [diagonal(tensordot(p1[i],p2[i],1)).T for i in range(len(pos))]

  y = [average(sqrt(sum(((-0.5/R_0[i].reshape(R_0[i].shape+(1,))*dp[i]*p1[i].reshape(p1[i].shape+(1,)).transpose(1,0,2))-p2[i])**2,0)),0) for i in range(len(pos))]

  plt.title("Average displacement from straight solution")
  plt.xlabel("Time / hour")
  plt.ylabel("Distance / km")
  for i in range(len(y)): plt.plot(arange(0,100), y[i])
  plt.show()

  return "Success."

def ComputeForce(savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return "Error loading: %s" % str(e)

  pos = [array(d["Position"]) for d in data]
  sprk = [array(d["SprK"]) for d in data]

  y = [sprk[i][:-1]*average(sqrt(sum((pos[i][:,1:]-pos[i][:,:-1])**2,2)),0) for i in range(len(pos))]

  plt.title("Average tension density over the string")
  plt.xlabel("Node")
  plt.ylabel("Force density / 10$^3$ N/km")
  for i in range(len(y)): plt.plot(arange(0,99), y[i]*1e3/3600**2/1e3/average(sqrt(sum((pos[i][:,1:]-pos[i][:,:-1])**2,2)),0))
  plt.show()

  return "Success."

def LongModesCW(savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return "Error loading: %s" % str(e)

  pos = [array(d["Position"]) for d in data]
  t = linspace(0,64,2048)
  #dt = t[1]-t[0]
  dt = data[0]["Dt"]

  yt = [sqrt(sum(p[:,-1]**2,1)) for p in pos]
  #yt = [sin(5*pi*t)+cos(10*pi*t) for p in pos]
  yw = [rfftn(yt[i]) for i in range(len(pos))]
  yw2 = [abs(yw[i][1:int(0.5*yw[i].size)+1]) for i in range(len(pos))]
  w = [rfftfreq(yw[i].size, dt)[1:] for i in range(len(yw2))]

  print(dt)
  print(yw2[0].shape)
  print(w[0].shape)
  print(yt[0].shape)

  plt.title("Radial modes of vibration - Counter-Weight")
  plt.xlabel("Frequency / hour$^{-1}$")
  plt.ylabel("Amplitude")
  for i in range(len(yw2)): plt.plot(w[i], yw2[i]/max(yw2[i]))
  plt.show()

  return "Success."

def StringModes(savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return "Error loading: %s" % str(e)

  pos = [array(d["Position"]) for d in data]
  t = [array(d["Time"]) for d in data]

  th = [arctan2(p[:,-1,0]-p[:,0,0],p[:,-1,2]-p[:,0,2]) for p in pos]

  rotpos = [zeros_like(p) for p in pos]
  for i in range(len(pos)):
    rotpos[i][:,:,0] = cos(th[i]).reshape(th[i].shape+(1,))*(pos[i][:,:,0]-pos[i][:,0,0].reshape(th[i].shape+(1,))) - sin(th[i]).reshape(th[i].shape+(1,))*(pos[i][:,:,2]-pos[i][:,0,2].reshape(th[i].shape+(1,)))
    rotpos[i][:,:,2] = sin(th[i]).reshape(th[i].shape+(1,))*(pos[i][:,:,0]-pos[i][:,0,0].reshape(th[i].shape+(1,))) + cos(th[i]).reshape(th[i].shape+(1,))*(pos[i][:,:,2]-pos[i][:,0,2].reshape(th[i].shape+(1,)))

  yt = [p for p in pos]
  yw = [rfft(yt[i],axis=0) for i in range(len(pos))]
  yw2 = [abs(yw[i][1:int(0.5*yw[i].shape[0])+1]) for i in range(len(pos))]
  w = [rfftfreq(yw[i].shape[0], data[i]["Dt"])[1:] for i in range(len(yw2))]

  yk = [rfft(rotpos[i][:,:,0],axis=0) for i in range(len(pos))]
  yk2 = [abs(yk[i][1:int(0.5*yk[i].shape[0])+1]) for i in range(len(pos))]
  k = [rfftfreq(yk[i].shape[0], data[i]["Dt"])[1:] for i in range(len(yw2))]

  print(data[i]["Dt"])
  print(yt[0].shape)
  print(yw[0].shape)
  print(yw2[0].shape)
  print(w[0].shape)

  """
  for i in range(len(yw2)):
    plt.plot(w[i], yw2[i][:,0,0],'r-')
    plt.plot(w[i], yw2[i][:,0,1],'b-')
    plt.plot(w[i], yw2[i][:,0,2],'g-')
  """
  """
  plt.title("String perturbations evolution with respect to straight solution")
  plt.xlabel("$\sigma$ / Dimensionless")
  plt.ylabel("Perturbations/Length / Dimensionless")
  for i in range(len(yw2)):
    for j in linspace(0,1024,6):
      plt.plot(rotpos[i][int(j),:,2]/max(rotpos[i][int(j),:,2]), rotpos[i][int(j),:,0]/max(rotpos[i][int(j),:,2]),'-',label="Time step: %d" % int(j))
  plt.legend(loc=0)
  """
  """
  avg_p = [mean(rotpos[i][:,:,0]/amax(rotpos[i][:,:,2]), axis=1) for i in range(len(pos))]
  plt.title("Average string perturbations from the straight solution")
  plt.xlabel("Time / hour")
  plt.ylabel("Average perturbations over length / dimensionless")
  for i in range(len(yw2)):
    plt.plot(data[i]["Time"], avg_p[i], 'b-')
  """
  """
  for i in range(len(yw2)):
    plt.plot(pos[i][:,0,0], pos[i][:,0,2],'g-')
    plt.plot(pos[i][:,-1,0], pos[i][:,-1,2],'r-')
    for j in linspace(0,len(data[i]["Time"]),11)[:-1]:
      plt.plot(rotpos[i][int(j),:,0], pos[i][int(j),:,2],'k-')
  plt.axis("equal")
  """
  """
  for i in range(len(yw2)):
    for j in linspace(0,len(data[i]["Time"]),11)[:-1]:
      plt.plot(rotpos[i][int(j),:,2]/rotpos[i][int(j),-1,2], rotpos[i][int(j),:,0],'-')
  """
  """
  for i in range(len(yw2)):
    plt.plot(k[i], average(yk2[i],axis=1), '--')
    for j in linspace(0,99,10):
      plt.plot(k[i], yk2[i][:,int(j)], '-')
  """
  """
  n = 0
  # First set up the figure, the axis, and the plot element we want to animate
  fig = plt.figure()
  #ax = plt.axes(xlim=(k[n][0], k[n][-1]), ylim=(0, amax(yk2[n])))
  ax = plt.axes(xlim=(0, 1), ylim=(amin(rotpos[n][:,:,0]), amax(rotpos[n][:,:,0])))
  line, = ax.plot([], [], lw=2)

  # initialization function: plot the background of each frame
  def init():
    line.set_data([], [])
    return line,

  # animation function.  This is called sequentially
  def animate(i):
    ax.set_title("Time: %3.1f" % t[n][i])
    line.set_data(rotpos[n][i,:,2]/rotpos[n][i,-1,2], rotpos[n][i,:,0])
    return line,

  # call the animator.  blit=True means only re-draw the parts that have changed.
  anim = animation.FuncAnimation(fig, animate, init_func=init, frames=t[n].size, interval=10, blit=True)
  plt.show()
  """
  #"""
  M = 512
  for i in range(4):
    plt.subplot(4,1,i+1)
    if i==0: plt.title("String perturbations evolution from straight solution")
    plt.imshow(rotpos[0][i*M:(i+1)*M,:,0].T,extent=(data[0]["Time"][i*M],data[0]["Time"][(i+1)*M],1.0,0.0))
    plt.ylabel("$\sigma$")
  #"""
  plt.xlabel("Time / hour")
  plt.show()

  return "Success."

def StringModes2(savefile, filenames):
  nt = 2**10
  nx = 2**11

  t = linspace(0, 1.0, nt)
  x = linspace(0, 1.0, nx)

  [X, T] = meshgrid(x, t)

  yx = cos(50*2*pi*X*T)*sin(25*2*pi*T*X)

  yk = abs(fft2(yx))
  k = fftfreq(yk.shape[0], x[1]-x[0])[1:]
  w = fftfreq(yk.shape[1], t[1]-t[0])[1:]

  print(yk.shape)
  print(k.shape)
  print(w.shape)

  plt.subplot(211)
  plt.imshow(yx, extent=(x[0],x[-1],t[0],t[-1]))
  plt.subplot(212)
  plt.imshow(log(yk[1:nx//2+1,1:nt//2+1]), extent=(w[0],w[-1],k[0],k[-1]))
  plt.show()

  return "Success."

def StringLength(savefile, filenames):
  try:
    data = []
    for filename in filenames:
      f = open(filename,'r')
      data.append(json.loads(f.read()))
      f.close()
  except Exception as e:
    return "Error loading: %s" % str(e)

  pos = [array(d["Position"]) for d in data]
  y = [sum(sqrt(sum((p[:,:-1]-p[:,1:])**2,2)),1) for p in pos]

  plt.title("String length over time")
  plt.xlabel("Time / hour")
  plt.ylabel("Distance / km")
  for i in range(len(y)): plt.plot(data[i]["Time"], y[i])
  plt.show()

  return "Success."

def ParseInput(argv):
  if len(argv) > 1:
    if argv[1] == "-cwd":
      print(CWDistance(sys.argv[2], sys.argv[3:]))
    elif argv[1] == "-avd":
      print(AverageDisplacement(sys.argv[2], sys.argv[3:]))
    elif argv[1] == "-force":
      print(ComputeForce(sys.argv[2], sys.argv[3:]))
    elif argv[1] == "-cwm":
      print(LongModesCW(sys.argv[2], sys.argv[3:]))
    elif argv[1] == "-sm":
      print(StringModes(sys.argv[2], sys.argv[3:]))
    elif argv[1] == "-len":
      print(StringLength(sys.argv[2], sys.argv[3:]))
    else:
      print("Wrong argument.")

ParseInput(sys.argv)
