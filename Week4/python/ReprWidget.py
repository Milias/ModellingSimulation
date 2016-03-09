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
    self.Grid.addWidget(bload,1,3,1,1)

    self.Grid.addWidget(QLabel("<b>2D representation</b>"), 1, 0, 1, 4, Qt.AlignHCenter)

    dummy = QWidget()
    dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    self.Grid.addWidget(dummy,100,0)

  def On3DRepr(self):
    ret = subprocess.Popen(["./python/graphs.py","-plot3",self.DataWidgets["3dFileName"].text()],stdout=subprocess.PIPE)
    self.Parent.statusBar().showMessage("Running representation...", 1500)

  def OnPackFracStep(self):
    try:
      PlotPackingFractionVsStep(self.DataWidgets["3dFileName"].text())
    except Exception as e:
      self.Parent.statusBar().showMessage(str(e), 5000)
