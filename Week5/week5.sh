#!/bin/bash

./bin/week5 -bhist ../Week4/data/evolution-fcc-fluid.json 0.0 2.5 100 data/hist-fcc.json
python3 python/graphs.py -hist data/hist-fcc.json 100
#./bin/week5 -dhist data/evolution-fcc-5.0.json 0.0 4.0 0.5 data/hist-fcc-5.0.json
#./bin/week5 -bhist data/evolution-fcc-1.5.json 0.0 10.0 10 data/hist-fcc-1.5.json
