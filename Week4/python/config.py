#!/bin/python
# -*- coding: utf8 -*-
from ConfigWidget import *
from LatticeWidget import *
from EvolveWidget import *
from ReprWidget import *
from JobsWidget import *

class ConfGen(QMainWindow):
  def __init__(self):
    super().__init__()
    self.setCentralWidget(QWidget())
    self.Init()

    self.TabOrdering = {0 : "General", 1 : "Lattice", 2 : "Evolve", 3 : "Jobs", 4 : "Representation"}
    self.TabWidgetTypes = {"General" : ConfigWidget, "Lattice" : LatticeWidget, "Evolve" : EvolveWidget, "Jobs" : JobsWidget, "Representation" : RepresentationWidget}
    self.UpdateFunctions = {}

    for i in range(len(self.TabOrdering)): self.InitTab(i)

    self.resize(512,700)
    self.center()
    self.setWindowTitle('Monte Carlo simulation settings')
    self.show()

  def Init(self):
    self.TabWidgets = {}

    grid = QGridLayout()
    self.centralWidget().setLayout(grid)

    self.TabBox = QTabWidget(self)
    self.TabBox.currentChanged[int].connect(self.UpdateTab)
    grid.addWidget(self.TabBox, 0, 0)

  def InitTab(self, index):
    name = self.TabOrdering[index]
    self.TabWidgets[name] = self.TabWidgetTypes[name](name,self)
    self.TabBox.addTab(self.TabWidgets[name], name)

  def UpdateTab(self, index):
    name = self.TabOrdering[index]
    if name in self.UpdateFunctions:
      self.UpdateFunctions[name]()

  def center(self):
    qr = self.frameGeometry()
    cp = QDesktopWidget().availableGeometry().center()
    qr.moveCenter(cp)
    self.move(qr.topLeft())

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = ConfGen()
    sys.exit(app.exec_())
