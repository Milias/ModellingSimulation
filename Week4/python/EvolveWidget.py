# -*- coding: utf8 -*-
import json

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

class EvolveWidget(QWidget):
  def __init__(self, name, parent):
    super().__init__(parent.TabBox)

    self.Grid = QGridLayout()
    self.Grid.setSpacing(10)
    self.setLayout(self.Grid)

    self.DataWidgets = {}
    self.Parent = parent
    self.Name = name
    self.Init()

    self.Parent.UpdateFunctions[self.Name] = self.OnPreviewJob

  def Init(self):
    self.Grid.addWidget(QLabel("<b>Job settings</b>"), 0, 0, 1, 4, Qt.AlignHCenter)
    self.Grid.addWidget(QLabel("<b>Job preview</b>"), 4, 0, 1, 4, Qt.AlignHCenter)

    self.Grid.addWidget(QLabel("Configuration file"), 1, 0)
    line_edit = QLineEdit()
    line_edit.setText("data/config.json")
    line_edit.textEdited[str].connect(self.OnPreviewJob)
    self.DataWidgets["ConfigFile"] = line_edit
    self.Grid.addWidget(line_edit, 1, 1, 1, 3)

    self.Grid.addWidget(QLabel("Lattice file"), 2, 0)
    line_edit = QLineEdit()
    line_edit.textEdited[str].connect(self.OnPreviewJob)
    line_edit.setText("data/lattice-sc.json")
    self.DataWidgets["LatticeFile"] = line_edit
    self.Grid.addWidget(line_edit, 2, 1, 1, 3)

    self.Grid.addWidget(QLabel("Output file"), 3, 0)
    line_edit = QLineEdit()
    line_edit.textEdited[str].connect(self.OnPreviewJob)
    line_edit.setText("data/evolution-sc.json")
    self.DataWidgets["OutputFile"] = line_edit
    self.Grid.addWidget(line_edit, 3, 1, 1, 3)

    preview = QWidget()
    self.PreviewGrid = QGridLayout()
    preview.setLayout(self.PreviewGrid)
    self.Grid.addWidget(preview, 5, 0)

    self.JobPreviewEntries = ["TotalSteps", "SaveSystemInterval", "StepSize", "ParticleMoves", "VolumeDelta", "VolumeChanges", "BPSigma", "Dimensions", "SpheresNumber", "SphereSize"]
    entries_text = ["Simulation steps", "Save interval", "Step size", "Particle moves per step", "Volume delta", "Volume changes per step", "βPσ^3", "Dimensions", "Spheres #", "Spheres radius"]

    for i, n in enumerate(self.JobPreviewEntries):
      label = QLabel(entries_text[i])
      label.setAlignment(Qt.AlignRight)
      self.PreviewGrid.addWidget(label, i + 6, 0)
      label = QLabel()
      self.DataWidgets[n] = label
      self.PreviewGrid.addWidget(label, i + 6, 1, 1, 3)

    bjob = QPushButton("Queue job")
    bjob.clicked.connect(self.OnCreateJob)
    self.Grid.addWidget(bjob,100,0,1,4)

    dummy = QWidget()
    dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    self.Grid.addWidget(dummy,99,0)

    self.OnPreviewJob()

  def OnPreviewJob(self):
    try:
      f = open(self.DataWidgets["ConfigFile"].text(),'r')
      data = json.loads(f.read())
      f.close()
      f = open(self.DataWidgets["LatticeFile"].text(),'r')
      data.update(json.loads(f.read()))
      f.close()
    except Exception as e:
      return

    for n in self.JobPreviewEntries:
      self.DataWidgets[n].setText(str(data[n]))

  def OnCreateJob(self):
    try:
      f = open(self.DataWidgets["ConfigFile"].text(),'r')
      f.close()
      f = open(self.DataWidgets["LatticeFile"].text(),'r')
      f.close()
    except Exception as e:
      self.Parent.statusBar().showMessage(str(e), 5000)
      return
