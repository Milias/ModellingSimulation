# -*- coding: utf8 -*-
import json

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

class ConfigWidget(QWidget):
  def __init__(self, name, parent):
    super().__init__(parent.TabBox)
    self.InputNames = ["TotalSteps", "SaveSystemInterval", "StepSize", "ParticleMoves", "VolumeDelta", "VolumeChanges", "BPSigma", "ConfigSavedFile"]

    self.Grid = QGridLayout()
    self.Grid.setSpacing(10)
    self.setLayout(self.Grid)

    self.DataWidgets = {}
    self.Parent = parent
    self.Name = name
    self.Init()

  def Init(self):
    entries = ["Simulation steps", "Save interval", "Step size", "Particle moves per step", "Volume delta", "Volume changes per step", "βPσ^3", "Save file"]
    validators = [QIntValidator(), QIntValidator(), QDoubleValidator(), QIntValidator(), QDoubleValidator(), QIntValidator(), QDoubleValidator(), None]
    def_text = ["10000", "1000", "0.1", "10", "0.1", "1", "10", "data/config.json"]
    loc = [1, 2, 3, 4, 7, 8, 9, 10]

    self.Grid.addWidget(QLabel("<b>NVT ensemble</b>", self), 0, 0, 1, 2, Qt.AlignHCenter)
    self.Grid.addWidget(QLabel("<b>NPT ensemble</b>", self), 5, 0, 1, 2, Qt.AlignHCenter)
    self.Grid.addWidget(QLabel("<i>(set volume changes to zero to use the NVT ensemble)</i>", self), 6, 0, 1, 2, Qt.AlignHCenter)

    for i, n in enumerate(entries):
      line_edit = QLineEdit(self)
      line_edit.setValidator(validators[i])
      line_edit.setText(def_text[i])
      self.Grid.addWidget(QLabel(n, self),loc[i],0)
      self.Grid.addWidget(line_edit,loc[i],1)
      self.DataWidgets[self.InputNames[i]] = line_edit

    bsave = QPushButton("Save",self)
    bsave.clicked.connect(self.OnSaveConfiguration)
    self.Grid.addWidget(bsave,loc[-1]+2,0,1,1)

    bsave = QPushButton("Load",self)
    bsave.clicked.connect(self.OnLoadConfiguration)
    self.Grid.addWidget(bsave,loc[-1]+2,1,1,1)

    self.OnLoadConfiguration()

    dummy = QWidget(self)
    dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    self.Grid.addWidget(dummy,loc[-1]+1,0)

  def OnLoadConfiguration(self):
    try:
      f = open(self.DataWidgets["ConfigSavedFile"].text(),'r')
      data = json.loads(f.read())
      f.close()
      self.Parent.statusBar().showMessage("Loaded previous configuration file.", 1500)
    except:
      return None

    for i in self.InputNames:
      if i == "ConfigSavedFile": continue
      self.DataWidgets[i].setText(str(data[i]))

  def OnSaveConfiguration(self):
    var_type = {"TotalSteps" : int, "SaveSystemInterval" : int, "StepSize" : float, "ParticleMoves" : int, "VolumeDelta" : float, "VolumeChanges" : int, "BPSigma" : float, "ConfigSavedFile" : None}
    data = {}
    for i in self.InputNames:
      if var_type[i] == None: continue
      data[i] = var_type[i](self.DataWidgets[i].text())
    data["FileType"] = "HardSpheresInit"

    try:
      savedfile = open(self.DataWidgets["ConfigSavedFile"].text(), 'w+')
      savedfile.write(json.dumps(data))
      savedfile.close()
      self.Parent.statusBar().showMessage("Saved to %s successfully." % self.DataWidgets["ConfigSavedFile"].text(), 1500)
    except Exception as e:
      self.Parent.statusBar().showMessage(str(e), 5000)
