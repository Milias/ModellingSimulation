# -*- coding: utf8 -*-
import json
import copy

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

    self.Grid.addWidget(QLabel("System settings"), 1, 0, 1, 1)
    line_edit = QLineEdit()
    line_edit.setText("data/config.json")
    line_edit.textEdited[str].connect(self.OnPreviewJob)
    self.DataWidgets["ConfigFile"] = line_edit
    self.Grid.addWidget(line_edit, 1, 1, 1, 2)
    bsearch = QPushButton("...")
    bsearch.clicked.connect(self.SearchFiles)
    self.FileDialog = QFileDialog()
    self.SelectedConfigFiles = ["data/config.json"]
    self.Grid.addWidget(bsearch, 1, 3, 1, 1)

    self.InputMode = QComboBox()
    self.InputMode.addItem("Lattice")
    self.InputMode.addItem("Evolution")
    self.Grid.addWidget(self.InputMode, 2, 0, 1, 1)
    line_edit = QLineEdit()
    line_edit.setText("data/lattice-fcc.json")
    line_edit.textEdited[str].connect(self.OnPreviewJob)
    self.DataWidgets["LatticeFile"] = line_edit
    self.Grid.addWidget(line_edit, 2, 1, 1, 3)

    self.Grid.addWidget(QLabel("Output"), 3, 0, 1, 1)
    line_edit = QLineEdit()
    line_edit.setText("data/evolution-fcc.json")
    self.DataWidgets["OutputFile"] = line_edit
    self.Grid.addWidget(line_edit, 3, 1, 1, 3)

    preview = QWidget()
    self.PreviewGrid = QGridLayout()
    preview.setLayout(self.PreviewGrid)
    self.Grid.addWidget(preview, 5, 0, 1, 4)

    self.JobPreviewEntries = ["TotalSteps", "SaveSystemInterval", "ParticleMoves", "VolumeChanges", "BPSigma", "Dimensions", "SpheresNumber", "SphereSize", "JobsNumber"]
    entries_text = ["Simulation steps", "Save interval", "Particle moves per step", "Volume changes per step", "βPσ^3", "Dimensions", "Spheres #", "Spheres radius", "Jobs #"]

    for i, n in enumerate(self.JobPreviewEntries):
      label = QLabel(entries_text[i])
      label.setAlignment(Qt.AlignRight)
      self.PreviewGrid.addWidget(label, i + 6, 0, 1, 2)
      label = QLabel()
      self.DataWidgets[n] = label
      self.PreviewGrid.addWidget(label, i + 6, 2, 1, 2)

    bjob = QPushButton("Queue job(s)")
    bjob.clicked.connect(self.OnCreateJob)
    self.Grid.addWidget(bjob,100,0,1,4)

    dummy = QWidget()
    dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    self.Grid.addWidget(dummy,99,0)

    self.OnPreviewJob()

  def SearchFiles(self):
    self.SelectedConfigFiles[:] = self.FileDialog.getOpenFileNames(self, "Select configuration file(s)", "data", "JSON files (*.json)")[0]

    to_remove = []
    for i, filename in enumerate(self.SelectedConfigFiles):
      try:
        file_data = open(filename,'r')
        temp_data = json.loads(file_data.read())
        file_data.close()
        if not "FileType" in temp_data:
          to_remove.append(i)
          continue

        if temp_data["FileType"] != "HardSpheresInit": to_remove.append(i)
      except Exception as e:
        return

    for i in reversed(to_remove): del self.SelectedConfigFiles[i]

    if len(self.SelectedConfigFiles) == 1:
      self.DataWidgets["ConfigFile"].setText(self.SelectedConfigFiles[0])
    elif len(self.SelectedConfigFiles) > 1:
      self.DataWidgets["ConfigFile"].setText("(several files)")
    self.OnPreviewJob()

    if len(self.SelectedConfigFiles) > 0:
      self.Parent.statusBar().showMessage("Loaded %d file(s)." % len(self.SelectedConfigFiles), 1500)
    else:
      self.Parent.statusBar().showMessage("No files loaded.", 2500)

  def OnPreviewJob(self):
    if len(self.SelectedConfigFiles) == 0: return
    try:
      data = {}
      for filename in self.SelectedConfigFiles:
        file_data = open(filename,'r')
        temp_data = json.loads(file_data.read())
        file_data.close()
        for key in temp_data:
          if key in data:
            if temp_data[key] != data[key]:
              data[key] = "(several values)"
          else:
            data[key] = temp_data[key]

      file_data = open(self.DataWidgets["LatticeFile"].text(),'r')
      data.update(json.loads(file_data.read()))
      file_data.close()
    except Exception as e:
      return

    for n in self.JobPreviewEntries:
      if n == "JobsNumber":
        self.DataWidgets[n].setText(str(len(self.SelectedConfigFiles)))
      else:
        self.DataWidgets[n].setText(str(data[n]))

  def OnCreateJob(self):
    if self.InputMode.currentIndex() == 0:
      mode = "-gevolve"
    else:
      mode = "-levolve"

    filename = self.DataWidgets["OutputFile"].text()
    if len(self.SelectedConfigFiles) > 1:
      try:
        names = [filename % i for i in range(len(self.SelectedConfigFiles))]
      except Exception as e:
        self.Parent.statusBar().showMessage("Error: %s" % str(e), 5000)
        return
    else:
      names = [filename]

    job_data = {
      "BinaryFile": "./bin/week4",
      "InputMode" : mode,
      "ConfigFile": "",
      "LatticeFile": self.DataWidgets["LatticeFile"].text(),
      "OutputFile": ""
      }

    for i, n in enumerate(self.SelectedConfigFiles):
      job_data["ConfigFile"] = n
      job_data["OutputFile"] = names[i]
      self.Parent.TabWidgets["Jobs"].RegisterJob(job_data)
      job_data = copy.deepcopy(job_data)
