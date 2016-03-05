# -*- coding: utf8 -*-
from lattice import *

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *

class SaveLatticeThread(QThread):
  result = pyqtSignal(str, int)

  def __init__(self, func):
    super().__init__()

    self.TargetFunction = func

  def run(self):
    self.result.emit(self.TargetFunction(), 5000)

class LatticeWidget(QWidget):
  def __init__(self, name, parent):
    super().__init__(parent.TabBox)
    self.InputNames = ["Dimensions", "SpheresNumber", "SphereSize", "Basis"]

    self.Grid = QGridLayout()
    self.Grid.setSpacing(10)
    self.setLayout(self.Grid)

    self.DataWidgets = {}
    self.Parent = parent
    self.Name = name

    self.ThreadSaveLattice = SaveLatticeThread(self.OnSaveLattice)
    self.ThreadSaveLattice.started.connect(lambda: self.Parent.statusBar().showMessage("Working..."))
    self.ThreadSaveLattice.result[str,int].connect(lambda s, t: self.Parent.statusBar().showMessage(s,t))
    self.ThreadSaveLattice.finished.connect(self.UpdateLatticeInfo)

    self.Init()

  def Init(self):
    combo_entries = ["Rectangular", "Hexagonal", "Simple Cubic", "Face-Centered Cubic"]
    self.ChosenLattice = combo_entries[0]

    self.Grid.addWidget(QLabel("<b>Lattice generation</b>"), 0, 0, 1, 4, Qt.AlignHCenter)
    combo_box = QComboBox()
    for n in combo_entries:
      combo_box.addItem(n)

    combo_box.activated[str].connect(self.OnChosenLattice)
    self.Grid.addWidget(QLabel("Lattice type"), 1, 0)
    self.Grid.addWidget(combo_box, 1, 1, 1, 4)

    self.Grid.addWidget(QLabel("Unit cells per dimension"), 2, 0)
    for i in range(3):
      line_edit = QLineEdit()
      line_edit.setValidator(QIntValidator())
      line_edit.setText("10")
      self.DataWidgets["Lattice-N%d" % i] = line_edit
      self.Grid.addWidget(line_edit, 2, i+1)
    self.DataWidgets["Lattice-N2"].setReadOnly(True)

    self.Grid.addWidget(QLabel("Sphere radius"), 3, 0)
    line_edit = QLineEdit()
    line_edit.setValidator(QDoubleValidator())
    line_edit.setText("1.0")
    self.DataWidgets["SphereSize"] = line_edit
    self.Grid.addWidget(line_edit, 3, 1, 1, 3)

    self.Grid.addWidget(QLabel("Scale factor"), 4, 0)
    line_edit = QLineEdit()
    line_edit.setValidator(QDoubleValidator())
    line_edit.setText("2.0")
    self.DataWidgets["LatticeScale"] = line_edit
    self.Grid.addWidget(line_edit, 4, 1, 1, 3)

    self.Grid.addWidget(QLabel("Save file"), 5, 0)
    line_edit = QLineEdit()
    line_edit.setText("data/lattice-rectangular.json")
    line_edit.textEdited[str].connect(self.UpdateLatticeInfo)
    self.DataWidgets["LatticeSavedFile"] = line_edit
    self.Grid.addWidget(line_edit, 5, 1, 1, 3)

    bsave = QPushButton("Save")
    bsave.clicked.connect(lambda: self.ThreadSaveLattice.start())
    self.Grid.addWidget(bsave,6,0,1,4)

    self.LatticeInformation = QWidget(self)
    self.LatticeInfoGrid = QGridLayout(self.LatticeInformation)
    self.LatticeInformation.setLayout(self.LatticeInfoGrid)
    self.Grid.addWidget(self.LatticeInformation, 9, 0, 4, 4)

    self.LatticeInfoGrid.addWidget(QLabel("<b>Lattice information</b>"), 0, 0, 1, 2, Qt.AlignHCenter)

    self.DataWidgets["LatticeInfo"] = {}

    self.LatticeInfoGrid.addWidget(QLabel("Dimensions", self.LatticeInformation), 1, 0)
    label = QLabel()
    self.DataWidgets["LatticeInfo"]["Dimensions"] = label
    self.LatticeInfoGrid.addWidget(label, 1, 1)

    self.LatticeInfoGrid.addWidget(QLabel("Spheres #", self.LatticeInformation), 2, 0)
    label = QLabel()
    self.DataWidgets["LatticeInfo"]["SpheresNumber"] = label
    self.LatticeInfoGrid.addWidget(label, 2, 1)

    self.LatticeInfoGrid.addWidget(QLabel("Spheres radius", self.LatticeInformation), 3, 0)
    label = QLabel()
    self.DataWidgets["LatticeInfo"]["SphereSize"] = label
    self.LatticeInfoGrid.addWidget(label, 3, 1)

    self.UpdateLatticeInfo()

    dummy = QWidget()
    dummy.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    self.Grid.addWidget(dummy,8,0)

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
