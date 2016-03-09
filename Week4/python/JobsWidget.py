# -*- coding: utf8 -*-
import sys
import os
import subprocess
import json
import bz2

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

class GenerationJobThread(QThread):
  job_finished = pyqtSignal(int)
  progress_update = pyqtSignal(str)
  def __init__(self, completed, widget, data):
    super().__init__()

    self.Data = data
    self.Widget = widget
    self.job_finished[int].connect(completed)
    self.progress_update[str].connect(widget.ProcessUpdate)
    self.started.connect(widget.ProcessStarted)

  def run(self):
    args = [self.Data["BinaryFile"], self.Data["InputMode"], self.Data["ConfigFile"], self.Data["LatticeFile"], self.Data["OutputFile"]]
    with subprocess.Popen(args,stdout=subprocess.PIPE, bufsize=1, universal_newlines=True) as p:
      for line in p.stdout:
        self.progress_update.emit(line+"\n")
    self.job_finished.emit(self.Data["JobIndex"])

class GenerationJobWidget(QWidget):
  def __init__(self, data):
    super().__init__()
    self.Data = data
    self.InitWidget()

  def InitWidget(self):
    grid = QHBoxLayout()
    self.setLayout(grid)

    label = QLabel("Job %d" % self.Data["JobIndex"])
    grid.addWidget(label)

    self.ProgressPercent = QLabel("Waiting")
    grid.addWidget(self.ProgressPercent)

  def ProcessStarted(self):
    self.ProgressPercent.setText("Starting")

  def ProcessUpdate(self, progress):
    try:
      json_map = json.loads(progress)
    except:
      return

    if json_map["CurrentStep"] != json_map["TotalSteps"]:
      self.ProgressPercent.setText("%3.0f%%" % (100.0*json_map["CurrentStep"]/json_map["TotalSteps"]))
    else:
      self.ProgressPercent.setText("Saving")

class JobsWidget(QWidget):
  def __init__(self, name, parent):
    super().__init__(parent.TabBox)

    self.Grid = QGridLayout()
    self.Grid.setSpacing(10)
    self.setLayout(self.Grid)

    self.DataWidgets = {}
    self.PendingJobs = []
    self.CompletedJobs = []
    self.Parent = parent
    self.Name = name
    self.JobCount = 0
    self.Init()

  def Init(self):
    self.Grid.addWidget(QLabel("<b>Queued jobs</b>"), 0, 0, 1, 2, Qt.AlignHCenter)
    self.Grid.addWidget(QLabel("<b>Completed jobs</b>"), 0, 2, 1, 2, Qt.AlignHCenter)

    self.JobQueues = []
    for i in range(2):
      scroll_area = QScrollArea()
      scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

      widget = QWidget()
      widget.setLayout(QVBoxLayout())
      scroll_area.setWidget(widget)
      scroll_area.setWidgetResizable(True)

      dummy = QWidget()
      dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
      widget.layout().addWidget(dummy,10000)

      self.JobQueues.append(widget.layout())
      self.Grid.addWidget(scroll_area,1,2*i,1,2)

    bstart = QPushButton("Start")
    bstart.clicked.connect(self.StartJobs)
    self.Grid.addWidget(bstart, 2,0,1,2)

    bclear = QPushButton("Clear")
    bclear.clicked.connect(self.ClearJobs)
    self.Grid.addWidget(bclear, 2,2,1,2)

  def RegisterJob(self, data):
    data.update({"JobIndex":self.JobCount})
    job_widget = GenerationJobWidget(data)
    job_thread = GenerationJobThread(self.CompletedJob, job_widget, data)

    self.JobCount += 1
    self.JobQueues[0].insertWidget(len(self.PendingJobs), job_widget)
    self.PendingJobs.append(job_thread)

  def StartJobs(self):
    for i in self.PendingJobs: i.start()

  def ClearJobs(self):
    indices = [self.JobQueues[1].indexOf(i.Widget) for i in self.CompletedJobs]
    for i in reversed(indices):
      item = self.JobQueues[1].takeAt(i)
      if item != None:
        item.widget().deleteLater()

    del self.CompletedJobs[:]

  def CompletedJob(self, index):
    job = None
    for j in self.PendingJobs:
      if j.Data["JobIndex"] == index:
        job = j
        break

    if job == None: return

    self.JobQueues[0].removeWidget(job.Widget)
    self.PendingJobs.remove(job)

    self.JobQueues[1].insertWidget(len(self.CompletedJobs), job.Widget)
    self.CompletedJobs.append(job)
    job.Widget.ProgressPercent.setText("Done")
