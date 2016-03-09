# -*- coding: utf8 -*-
import sys
import os
import subprocess
import json

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

from graphs import *

class RepresentationWidget(QWidget):
  def __init__(self, name, parent):
    super().__init__(parent.TabBox)

    self.Grid = QGridLayout()
    self.Grid.setSpacing(10)
    self.setLayout(self.Grid)

    self.DataWidgets = {}
    self.Parent = parent
    self.Name = name
    self.Init()

  def Init(self):
    self.Grid.addWidget(QLabel("<b>3D representation</b>"), 0, 0, 1, 4, Qt.AlignHCenter)

    self.Grid.addWidget(QLabel("Data file"), 1, 0)
    line_edit = QLineEdit()
    line_edit.setValidator(QDoubleValidator())
    line_edit.setText("data/evolution-sc.json")
    self.DataWidgets["3dFileName"] = line_edit
    self.Grid.addWidget(line_edit, 1, 1, 1, 2)

    bload = QPushButton("Load")
    bload.clicked.connect(self.On3DRepr)
    self.Grid.addWidget(bload, 1, 3)

    self.Grid.addWidget(QLabel("<b>2D representation</b>"), 2, 0, 1, 4, Qt.AlignHCenter)

    self.SelectedFiles = []
    self.FileDialog = QFileDialog()
    self.Grid.addWidget(QLabel("Packing fraction vs step"), 3, 0)
    bsearch = QPushButton("...")
    bsearch.clicked.connect(self.OnSearchFiles)
    self.Grid.addWidget(bsearch, 3, 1)

    bplot = QPushButton("Plot")
    bplot.clicked.connect(self.OnPackFracStep)
    self.Grid.addWidget(bplot, 3, 2)

    self.Grid.addWidget(QLabel("Packing fraction vs P"), 4, 0)
    bsearch = QPushButton("...")
    bsearch.clicked.connect(self.OnSearchFiles)
    self.Grid.addWidget(bsearch, 4, 1)

    bplot = QPushButton("Plot")
    bplot.clicked.connect(self.OnPackFracP)
    self.Grid.addWidget(bplot, 4, 2)

    dummy = QWidget()
    dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    self.Grid.addWidget(dummy,100,0)

  def OnSearchFiles(self):
    self.SelectedFiles[:] = self.FileDialog.getOpenFileNames(self, "Select configuration file(s)", "data", "JSON files (*.json)")[0]

  def On3DRepr(self):
    ret = subprocess.Popen(["./python/graphs.py","-plot3",self.DataWidgets["3dFileName"].text()],stdout=subprocess.PIPE)
    self.Parent.statusBar().showMessage("Starting representation...", 1500)

  def OnPackFracStep(self):
    for i in self.SelectedFiles:
      try:
        PlotPackingFractionVsStep(i)
      except Exception as e:
        self.Parent.statusBar().showMessage(str(e), 5000)

    plt.legend(loc=0,numpoints=1)
    plt.show()

  def OnPackFracP(self):
    for i in self.SelectedFiles:
      try:
        PlotPackingFractionVsP(i)
      except Exception as e:
        self.Parent.statusBar().showMessage(str(e), 5000)

    #plt.legend(loc=0)
    plt.show()
