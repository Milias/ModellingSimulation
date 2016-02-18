# -*- coding: utf8 -*-
from numpy import *
import matplotlib.pyplot as plt
import json

#"""
files = ["data/rsquared.json", "data/prsquared.json"]
f = [ file(i) for i in files ]
f_str = [ i.read() for i in f ]
data = [ json.loads(i) for i in f_str ]

plt.plot(arange(data[0]["StepNumber"]), data[0]["Data"], 'r-', label="Without periodic b. c.")
plt.plot(arange(data[1]["StepNumber"]), data[1]["Data"], 'b-', label="With periodic b. c.")

plt.savefig("report/graphs/rsquared.eps")
plt.legend(loc=0)
plt.show()
#"""
#"""
files = ["data/randomwalk2.json", "data/prandomwalk2.json"]
f = [ file(i) for i in files ]
f_str = [ i.read() for i in f ]
data = [ json.loads(i) for i in f_str ]

plt.plot(data[0]["Data"][0], data[0]["Data"][1], 'r-', label="Without periodic b. c.")
plt.plot(data[1]["Data"][0], data[1]["Data"][1], 'b-', label="With periodic b. c.")

plt.savefig("report/graphs/randomwalk.eps")
plt.legend(loc=0)
plt.show()
#"""
