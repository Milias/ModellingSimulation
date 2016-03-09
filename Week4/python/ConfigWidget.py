# -*- coding: utf8 -*-
import json
from numpy import *

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

class ConfigWidget(QWidget):
  def __init__(self, name, parent):
    super().__init__(parent.TabBox)
    self.InputNames = ["TotalSteps", "SaveSystemInterval", "StepSize", "ParticleMoves", "VolumeDelta", "VolumeChanges", "ConfigSavedFile"]

    self.Grid = QGridLayout()
    self.Grid.setSpacing(10)
    self.setLayout(self.Grid)

    self.DataWidgets = {}
    self.Parent = parent
    self.Name = name
    self.Init()

  def Init(self):
    entries = ["Simulation steps", "Save interval", "Step size", "Particle moves per step", "Volume delta", "Volume changes per step", "Save file"]
    validators = [QIntValidator(), QIntValidator(), QDoubleValidator(), QIntValidator(), QDoubleValidator(), QIntValidator(), QDoubleValidator(), None]
    def_text = ["10000", "1000", "0.1", "10", "0.1", "1", "data/config.json"]
    loc = [1, 2, 3, 4, 7, 8, 10]

    self.Grid.addWidget(QLabel("<b>NVT ensemble</b>", self), 0, 0, 1, 4, Qt.AlignHCenter)
    self.Grid.addWidget(QLabel("<b>NPT ensemble</b>", self), 5, 0, 1, 4, Qt.AlignHCenter)
    self.Grid.addWidget(QLabel("<i>(set volume changes to zero to use the NVT ensemble)</i>", self), 6, 0, 1, 4, Qt.AlignHCenter)

    for i, n in enumerate(entries):
      line_edit = QLineEdit(self)
      line_edit.setValidator(validators[i])
      line_edit.setText(def_text[i])
      self.Grid.addWidget(QLabel(n, self),loc[i],0,1,1)
      self.Grid.addWidget(line_edit,loc[i],1,1,3)
      self.DataWidgets[self.InputNames[i]] = line_edit

    self.Grid.addWidget(QLabel("βPσ^3 <i>(from:to:#points)</i>"), 9, 0)
    line_edit = QLineEdit()
    line_edit.setText("1")
    line_edit.setValidator(QDoubleValidator())
    self.DataWidgets["BPSigma"] = line_edit
    self.Grid.addWidget(line_edit, 9, 1)
    line_edit = QLineEdit()
    line_edit.setText("10")
    line_edit.setValidator(QDoubleValidator())
    self.DataWidgets["BPSigma-to"] = line_edit
    self.Grid.addWidget(line_edit, 9, 2)
    line_edit = QLineEdit()
    line_edit.setText("1")
    line_edit.setValidator(QIntValidator())
    self.DataWidgets["BPSigma-points"] = line_edit
    self.Grid.addWidget(line_edit, 9, 3)

    bsave = QPushButton("Save",self)
    bsave.clicked.connect(self.OnSaveConfiguration)
    self.Grid.addWidget(bsave,loc[-1]+2,0,1,2)

    bsave = QPushButton("Load",self)
    bsave.clicked.connect(self.OnLoadConfiguration)
    self.Grid.addWidget(bsave,loc[-1]+2,2,1,2)

    self.OnLoadConfiguration()

    dummy = QWidget(self)
    dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    self.Grid.addWidget(dummy,loc[-1]+1,0,1,4)

  def OnLoadConfiguration(self):
    try:
      f = open(self.DataWidgets["ConfigSavedFile"].text(),'r')
      data = json.loads(f.read())
      f.close()
      self.Parent.statusBar().showMessage("Loaded previous configuration file.", 1500)
    except Exception as e:
      self.Parent.statusBar().showMessage(str(e), 5000)
      return None

    for i in self.InputNames:
      if i == "ConfigSavedFile": continue
      self.DataWidgets[i].setText(str(data[i]))

  def OnSaveConfiguration(self):
    var_type = {"TotalSteps" : int, "SaveSystemInterval" : int, "StepSize" : float, "ParticleMoves" : int, "VolumeDelta" : float, "VolumeChanges" : int, "ConfigSavedFile" : None}
    data = {}

    filename = self.DataWidgets["ConfigSavedFile"].text()
    npoints = int(self.DataWidgets["BPSigma-points"].text())
    P = linspace(float(self.DataWidgets["BPSigma"].text()), float(self.DataWidgets["BPSigma-to"].text()), npoints)

    if npoints > 1:
      try:
        names = [filename % i for i in range(npoints)]
      except Exception as e:
        print(e)
        return
    else:
      names = [filename]

    for i in self.InputNames:
      if var_type[i] == None or i == "BPSigma": continue
      data[i] = var_type[i](self.DataWidgets[i].text())
    data["FileType"] = "HardSpheresInit"

    try:
      for i, n in enumerate(names):
        savedfile = open(n, 'w+')
        data["BPSigma"] = P[i]
        savedfile.write(json.dumps(data))
        savedfile.close()

      if npoints > 1:
        self.Parent.statusBar().showMessage("Saved %d files." % npoints, 1500)
      else:
        self.Parent.statusBar().showMessage("Saved to %s." % filename, 1500)

    except Exception as e:
      self.Parent.statusBar().showMessage("Error: %s" % str(e), 5000)
