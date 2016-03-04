# -*- coding: utf8 -*-
import sys
import os

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

from graphs import *
from lattice import *

class SaveLatticeThread(QThread):
  result = pyqtSignal(str, int)

  def __init__(self, func):
    if sys.version_info < (3, 0):
      super(SaveLatticeThread, self).__init__()
    else:
      super().__init__()

    self.TargetFunction = func

  def run(self):
    self.result.emit(self.TargetFunction(), 5000)

class ConfGen(QMainWindow):
  def __init__(self):
    if sys.version_info < (3, 0):
      super(ConfGen, self).__init__()
    else:
      super().__init__()

    self.TabNames = ["General", "Lattice"]
    self.InputNames = {
      "General" :
        ["TotalSteps", "SaveSystemInterval", "StepSize", "ParticleMoves", "VolumeDelta", "VolumeChanges", "BPSigma", "ConfigSavedFile"],
      "LatticeInfo" :
        ["Dimensions", "SpheresNumber", "SphereSize", "Basis"]
    }

    self.DataWidgets = {}

    self.setCentralWidget(QWidget(self))
    self.InitTabs()
    self.InitConfig(self.TabNames[0])
    self.InitLattice(self.TabNames[1])

  def InitTabs(self):
    grid = QGridLayout()
    self.centralWidget().setLayout(grid)

    self.TabBox = QTabWidget(self)
    grid.addWidget(self.TabBox, 0, 0)

    self.TabGrids = {i : QGridLayout() for i in self.TabNames}
    self.TabWidgets = {i : QWidget(self.TabBox) for i in self.TabNames}
    for i in self.TabNames:
      self.TabWidgets[i].setLayout(self.TabGrids[i])
      self.TabBox.addTab(self.TabWidgets[i], i)

    self.resize(512,0)
    self.center()
    self.setWindowTitle('Monte Carlo simulation settings')
    self.show()

  def InitConfig(self, name):
    grid = self.TabGrids[name]
    widget = self.TabWidgets[name]
    grid.setSpacing(10)

    entries = ["Simulation steps", "Save interval", "Step size", "Particle moves per step", "Volume delta", "Volume changes", "βPσ^3", "Save file"]
    validators = [QIntValidator(), QIntValidator(), QDoubleValidator(), QIntValidator(), QDoubleValidator(), QIntValidator(), QDoubleValidator(), None]
    def_text = ["10000", "1000", "0.1", "10", "0.1", "1", "10", "data/config.json"]
    loc = [1, 2, 3, 4, 7, 8, 9, 10]

    grid.addWidget(QLabel("<b>NVT ensemble</b>", widget), 0, 0, 1, 2, Qt.AlignHCenter)
    grid.addWidget(QLabel("<b>NPT ensemble</b>", widget), 5, 0, 1, 2, Qt.AlignHCenter)
    grid.addWidget(QLabel("<i>(set volume changes to zero to use the NVT ensemble)</i>", widget), 6, 0, 1, 2, Qt.AlignHCenter)

    for i, n in enumerate(entries):
      line_edit = QLineEdit(widget)
      line_edit.setValidator(validators[i])
      line_edit.setText(def_text[i])
      grid.addWidget(QLabel(n, widget),loc[i],0)
      grid.addWidget(line_edit,loc[i],1)
      self.DataWidgets[self.InputNames[name][i]] = line_edit

    bsave = QPushButton("Save",widget)
    bsave.clicked.connect(self.OnSaveConfiguration)
    grid.addWidget(bsave,loc[-1]+1,0,1,2)

    dummy = QWidget(self.TabWidgets[name])
    p = QSizePolicy()
    p.setVerticalPolicy(QSizePolicy.Expanding)
    dummy.setSizePolicy(p)
    grid.addWidget(dummy,100,0)

  def OnSaveConfiguration(self):
    data = {}
    for i in self.InputNames["General"]:
      if i == "ConfigSavedFile": continue
      data[i] = self.DataWidgets[i].text()
    data["FileType"] = "HardSpheresInit"

    try:
      savedfile = open(self.DataWidgets["ConfigSavedFile"].text(), 'w+')
      savedfile.write(json.dumps(data))
      savedfile.close()
      self.statusBar().showMessage("Saved to %s successfully." % self.DataWidgets["ConfigSavedFile"].text(), 1500)
    except Exception as e:
      self.statusBar().showMessage(str(e), 5000)

  def InitLattice(self, name):
    grid = self.TabGrids[name]
    widget = self.TabWidgets[name]
    grid.setSpacing(10)

    combo_entries = ["Rectangular", "Hexagonal", "Simple Cubic", "Face-Centered Cubic"]
    self.ChosenLattice = combo_entries[0]
    loc = []

    grid.addWidget(QLabel("<b>Lattice generation</b>", widget), 0, 0, 1, 4, Qt.AlignHCenter)
    combo_box = QComboBox(widget)
    for i, n in enumerate(combo_entries):
      combo_box.addItem(n)

    combo_box.activated[str].connect(self.OnChosenLattice)
    grid.addWidget(QLabel("Lattice type"), 1, 0)
    grid.addWidget(combo_box, 1, 1, 1, 4)

    grid.addWidget(QLabel("Unit cells per dimension"), 2, 0)
    for i in range(3):
      line_edit = QLineEdit(widget)
      line_edit.setValidator(QIntValidator())
      line_edit.setText("10")
      self.DataWidgets["Lattice-N%d" % i] = line_edit
      grid.addWidget(line_edit, 2, i+1)
    self.DataWidgets["Lattice-N2"].setReadOnly(True)

    grid.addWidget(QLabel("Sphere radius"), 3, 0)
    line_edit = QLineEdit(widget)
    line_edit.setValidator(QDoubleValidator())
    line_edit.setText("1.0")
    self.DataWidgets["SphereSize"] = line_edit
    grid.addWidget(line_edit, 3, 1, 1, 3)

    grid.addWidget(QLabel("Scale factor"), 4, 0)
    line_edit = QLineEdit(widget)
    line_edit.setValidator(QDoubleValidator())
    line_edit.setText("2.0")
    self.DataWidgets["LatticeScale"] = line_edit
    grid.addWidget(line_edit, 4, 1, 1, 3)

    grid.addWidget(QLabel("Save file"), 5, 0)
    line_edit = QLineEdit(widget)
    line_edit.setText("data/lattice-%s.json" % combo_entries[0].lower())
    line_edit.textEdited[str].connect(self.UpdateLatticeInfo)
    self.DataWidgets["LatticeSavedFile"] = line_edit
    grid.addWidget(line_edit, 5, 1, 1, 3)

    self.ThreadSaveLattice = SaveLatticeThread(self.OnSaveLattice)
    self.ThreadSaveLattice.started.connect(lambda: self.statusBar().showMessage("Working..."))
    self.ThreadSaveLattice.result[str,int].connect(lambda s, t: self.statusBar().showMessage(s, t))
    self.ThreadSaveLattice.finished.connect(self.UpdateLatticeInfo)

    bsave = QPushButton("Save",widget)
    bsave.clicked.connect(lambda: self.ThreadSaveLattice.start())
    grid.addWidget(bsave,6,0,1,4)

    self.LatticeInformation = QWidget(widget)
    self.LatticeInfoGrid = QGridLayout(self.LatticeInformation)
    self.LatticeInformation.setLayout(self.LatticeInfoGrid)
    grid.addWidget(self.LatticeInformation, 9, 0, 4, 4)

    self.LatticeInfoGrid.addWidget(QLabel("<b>Lattice information</b>", widget), 0, 0, 1, 2, Qt.AlignHCenter)

    self.DataWidgets["LatticeInfo"] = {}
    self.LatticeInfoGrid.addWidget(QLabel("Dimension", self.LatticeInformation), 1, 0)
    label = QLabel("", self.LatticeInformation)
    self.DataWidgets["LatticeInfo"]["Dimensions"] = label
    self.LatticeInfoGrid.addWidget(label, 1, 1)

    self.LatticeInfoGrid.addWidget(QLabel("Spheres #", self.LatticeInformation), 2, 0)
    label = QLabel("", self.LatticeInformation)
    self.DataWidgets["LatticeInfo"]["SpheresNumber"] = label
    self.LatticeInfoGrid.addWidget(label, 2, 1)

    self.LatticeInfoGrid.addWidget(QLabel("Spheres radius", self.LatticeInformation), 3, 0)
    label = QLabel("", self.LatticeInformation)
    self.DataWidgets["LatticeInfo"]["SphereSize"] = label
    self.LatticeInfoGrid.addWidget(label, 3, 1)

    self.UpdateLatticeInfo()

    dummy = QWidget(self.TabWidgets[name])
    p = QSizePolicy()
    p.setVerticalPolicy(QSizePolicy.Expanding)
    dummy.setSizePolicy(p)
    grid.addWidget(dummy,8,0)

  def UpdateLatticeInfo(self):
    try:
      f = open(self.DataWidgets["LatticeSavedFile"].text(),'r')
      data = json.loads(f.read())
      f.close()
    except:
      self.LatticeInformation.hide()
      return None

    self.DataWidgets["LatticeInfo"]["Dimensions"].setNum(data["Dimensions"])
    self.DataWidgets["LatticeInfo"]["SpheresNumber"].setNum(data["SpheresNumber"])
    self.DataWidgets["LatticeInfo"]["SphereSize"].setNum(data["SphereSize"])
    self.LatticeInformation.show()
    del data

  def OnChosenLattice(self, entry):
    if entry == "Rectangular":
      self.DataWidgets["Lattice-N2"].setReadOnly(True)
      self.ChosenLattice = "Rectangular"
      self.DataWidgets["LatticeSavedFile"].setText("data/lattice-rectangular.json")
    elif entry == "Hexagonal":
      self.DataWidgets["Lattice-N2"].setReadOnly(True)
      self.ChosenLattice = "Hexagonal"
      self.DataWidgets["LatticeSavedFile"].setText("data/lattice-hexagonal.json")
    elif entry == "Simple Cubic":
      self.DataWidgets["Lattice-N2"].setReadOnly(False)
      self.ChosenLattice = "Simple Cubic"
      self.DataWidgets["LatticeSavedFile"].setText("data/lattice-sc.json")
    elif entry == "Face-Centered Cubic":
      self.DataWidgets["Lattice-N2"].setReadOnly(False)
      self.ChosenLattice = "Face-Centered Cubic"
      self.DataWidgets["LatticeSavedFile"].setText("data/lattice-fcc.json")
    self.UpdateLatticeInfo()

  def OnSaveLattice(self):
    args = [
      [
        int(self.DataWidgets["Lattice-N0"].text()),
        int(self.DataWidgets["Lattice-N1"].text()),
        int(self.DataWidgets["Lattice-N2"].text())
      ],
      float(self.DataWidgets["SphereSize"].text()),
      float(self.DataWidgets["LatticeScale"].text()),
      self.DataWidgets["LatticeSavedFile"].text()
    ]

    if self.ChosenLattice == "Rectangular":
      ret = GenerateRectangular(*args)

    elif self.ChosenLattice == "Hexagonal":
      ret = GenerateHexagonal(*args)

    elif self.ChosenLattice == "Simple Cubic":
      ret = GenerateSC(*args)

    elif self.ChosenLattice == "Face-Centered Cubic":
      ret = GenerateFCC(*args)

    return ret

  def center(self):
    qr = self.frameGeometry()
    cp = QDesktopWidget().availableGeometry().center()
    qr.moveCenter(cp)
    self.move(qr.topLeft())

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = ConfGen()
    sys.exit(app.exec_())
